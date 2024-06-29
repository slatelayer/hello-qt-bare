

module.exports = {encode, decode}


function encode (a) {
	return JSON.stringify(a)
}


function decode (a) {
	return JSON.parse(a)
}


// TODO: binary encoding.
// 
// I couldn't get Qt's protobuf generator to work (missing for some reason, even in src folder).
// I only see it nested under Grpc, but the docs show it standalone.
//
// I tried `tiny-buffer-rpc` (and `cborg`), which were problematic because of no bigint support.
//
