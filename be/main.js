
require('./runtime.js')
const { setTimeout, setInterval } = require('bare-timers')
const sleep = ms => new Promise(a=>setTimeout(a, ms))

// XXX can't get esm to work for some reason; tried every way I could think of!
//import {encode, decode} from './encoding'
const { encode, decode } = require('./encoding')


const green = "#BADE5B"


core.recv = function(data) {
	const {typ, ts, msg} = decode(data)
	
	switch(typ){
	case 'click':
		setTitle(`greet @${getName()}!`, green)
	}
}

core.ready()

console.log("throwing"); throw "thrown"

var nameIdx = 0
function getName(){
	const names = [
		"mafintosh",
		"andrewosh",
		"dmc",
		"paulo"
	]
	const name = names[nameIdx++]
	if (nameIdx === names.length) nameIdx = 0
	return name
}

;(async function main() {
	setColor("#111111")
	setTitle("hello, Holepunch")

	await sleep(500)

	setTitle("hello, Holepunch", green)

	await sleep(1000)

	showButton()
}());

function setColor(color){
	color = color || "#333333"
	const msg = {
		typ: 'color',
		msg: {color},
		ts: Date.now()
	}
	core.send(encode(msg))
}

function setTitle(title, color){
	title = `▊▊▋ ${title || ''}`
	color = color || "#999999"
	const msg = {
		typ: 'title',
		msg: {title, color},
		ts: Date.now()
	}
	core.send(encode(msg))
}

function showButton(){
	core.send(encode({typ: 'showButton'}))
}

setInterval(() => {
	const msg = {
		typ: "heartbeat",
		ts: Date.now(),
		msg: {}
	}
	core.send( encode(msg) )
}, 1000)

