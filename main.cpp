
#include <stdlib.h>
#include <vector>
#include <assert.h>

#include <uv.h>
#include <js.h>
#include <bare.h>

#include <concurrentqueue.h>

#include <QObject>
#include <QtLogging>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QByteArray>
#include <QMetaObject>

#define SLATER "slater"
#define CORE "core"
#define HOME "slater.local"
#define SLATES "▊▊▋ "

typedef std::function<void(void)> core_job_t;
typedef moodycamel::ConcurrentQueue<core_job_t> core_queue_t;

typedef std::function<void(const char* buf, size_t len)> core_send_t;
typedef std::function<void(const char* msg)> core_log_t;

static struct {
  uv_thread_t barethread;
  uv_sem_t init_sem;
  uv_async_t async;
  
  core_queue_t queue;

  js_env_t* env = nullptr;

  core_send_t send = nullptr;
  core_log_t log = nullptr;
} core;

static void _queue_job(core_job_t&& job) {
  core.queue.enqueue(std::move(job));
  int err = uv_async_send(&core.async);
  assert(err == 0);
}

class Core : public QObject {
  Q_OBJECT
  //QML_SINGLETON
  
  Q_SIGNALS:
    void recv (const QString str);

  public:
    void _recv (const char* buf, size_t len) {
      QString str = QString::fromUtf8(buf, (qsizetype)len);
      recv(str);
    }

    Q_INVOKABLE
    void send(const QString& qstr) {
      _queue_job([qstr](){
        js_env_t* env = core.env;

        js_value_t* global;
        js_get_global(env, &global);

        js_value_t* cor;
        js_get_named_property(env, global, CORE, &cor);

        const char* name = "recv";
        bool has_property;
        js_has_named_property(env, cor, name, &has_property);
        if (!has_property) {
          return;
        }

        js_value_t* fun;
        js_get_named_property(env, cor, name, &fun);

        js_value_t* js_str;
        js_create_string_utf8(env, (utf8_t *)qstr.toStdString().c_str(), qstr.length(), &js_str);

        js_handle_scope_t* scope;
        int hs = js_open_handle_scope(env, &scope);

        js_value_t* args[1] = { js_str };
        js_value_t* result;
        js_call_function(env, cor, fun, 1, args, &result);

        js_close_handle_scope(env, scope);
      });
    }
};

static js_value_t* _runtime_log(js_env_t* env, js_callback_info_t* info) {
  int err;

  size_t argc = 1;
  js_value_t* argv[1];

  err = js_get_callback_info(env, info, &argc, argv, nullptr, nullptr);
  assert(err == 0);

  size_t len = 0;
  err = js_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
  assert(err == 0);

  char* str = (char*)malloc(len + 1);
  err = js_get_value_string_utf8(env, argv[0], (utf8_t*)str, len + 1, nullptr);
  assert(err == 0);

  std::string prefix = SLATES;
  const char* msg = prefix.append(str).c_str();

  if (core.log != nullptr) {
    core.log(msg);
  }
  else {
    qDebug("%s", msg);
  }
  
  free(str);

  return nullptr;
}

static js_value_t* _runtime_send(js_env_t* env, js_callback_info_t* info) {
  int err;

  size_t argc = 1;
  js_value_t* argv[1];
  err = js_get_callback_info(env, info, &argc, argv, nullptr, nullptr);
  assert(err == 0);

  size_t len = 0;
  err = js_get_value_string_utf8(env, argv[0], nullptr, 0, &len);
  assert(err == 0);

  char* str = (char*)malloc(len + 1);
  err = js_get_value_string_utf8(env, argv[0], (utf8_t*)str, len + 1, nullptr);
  assert(err == 0);

  if (core.send != nullptr) {
    core.send(str, len);
  }

  return nullptr;
}

static js_value_t* _runtime_ready(js_env_t*, js_callback_info_t*) {
  uv_sem_post(&core.init_sem);
  return nullptr;
}

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  app.setApplicationName(SLATER);
  app.setOrganizationName(SLATER);
  app.setOrganizationDomain(HOME);

  QQmlApplicationEngine engine;

  Core* qcore(new Core);

  core.send = [qcore](const char* buf, size_t len){
    qcore->_recv(buf, len);
  };

  int err = uv_sem_init(&core.init_sem, 0);
  assert(err == 0);
  
  uv_thread_create(&core.barethread, [](void*) {
    int err;

    uv_loop_t loop;
    err = uv_loop_init(&loop);
    assert(err == 0);

    err = uv_async_init(&loop, &core.async, [](uv_async_t*) {
      core_job_t job;
      while (core.queue.try_dequeue(job)) {
        job();
      }
    });
    assert(err == 0);
    uv_unref((uv_handle_t*)&core.async);

    js_platform_t *platform;
    err = js_create_platform(&loop, NULL, &platform);
    assert(err == 0);

    bare_t *bare;
    err = bare_setup(&loop, platform, &core.env, 0, nullptr, nullptr, &bare);
    assert(err == 0);

    js_value_t* global;
    js_get_global(core.env, &global);
    js_value_t* cor;
    js_create_object(core.env, &cor);
    js_set_named_property(core.env, global, CORE, cor);
    {
      const char* name = "log";
      js_value_t* fn;
      err = js_create_function(core.env, name, -1, _runtime_log, nullptr, &fn);
      assert(err == 0);
      err = js_set_named_property(core.env, cor, name, fn);
      assert(err == 0);
    }
    {
      const char* name = "send";
      js_value_t* fn;
      err = js_create_function(core.env, name, -1, _runtime_send, nullptr, &fn);
      assert(err == 0);
      err = js_set_named_property(core.env, cor, name, fn);
      assert(err == 0);
    }
    {
      const char* name = "ready";
      js_value_t* fn;
      err = js_create_function(core.env, name, -1, _runtime_ready, nullptr, &fn);
      assert(err == 0);
      err = js_set_named_property(core.env, cor, name, fn);
      assert(err == 0);
    }

    err = bare_run(bare, "be/main.js", NULL);
    assert(err == 0);

    int exit_code;
    err = bare_teardown(bare, &exit_code);
    assert(err == 0);

    err = js_destroy_platform(platform);
    assert(err == 0);

    uv_close((uv_handle_t*)&core.async, nullptr);

    err = uv_run(&loop, UV_RUN_DEFAULT);
    assert(err == 0);

  }, nullptr);

  uv_sem_wait(&core.init_sem);
  uv_sem_destroy(&core.init_sem);

  QObject::connect(
    &engine,
    &QQmlApplicationEngine::objectCreationFailed,
    &app,
    []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection
  );

  //qmlRegisterSingletonInstance("local.slater", 1, 0, "Slater", qcore);
  engine.rootContext()->setContextProperty(CORE, qcore);
  engine.load("ui/Main.qml");

  return app.exec();
}

#include "main.moc"
