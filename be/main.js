
import './runtime.js'
import { setTimeout, setInterval } from 'bare-timers'

import { encode, decode } from './encoding'


const sleep = ms => new Promise(a=>setTimeout(a, ms))

const green = "#BADE5B"


core.recv = function(data) {
	const {typ, ts, msg} = decode(data)
	
	switch(typ){
	case 'click':
		setTitle(`greet @${getName()}!`, green)
	}
}

core.ready()

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

