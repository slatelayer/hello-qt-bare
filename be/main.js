

const { setTimeout, setInterval } = require('bare-timers')
const sleep = ms => new Promise(a=>setTimeout(ms,a()))


// XXX can't get esm to work for some reason; tried every way I could think of!
//import {encode, decode} from './encoding'
const { encode, decode } = require('./encoding')


core.recv = function(data) {
	console.log("bare recv")
	
	const msg = decode(data)
	console.log(msg.msg)
}


setInterval(() => {
	const msg = {
		t: "heartbeat",
		d: Date.now(),
		m: "alive"
	}

	core.send( encode(msg) )

}, 5000)


core.ready()

