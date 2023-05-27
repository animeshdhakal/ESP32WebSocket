const HEADER_SIZE = 4;

const create_message = (msg_type, ...args) => {
	let msg = args.map(String).join("\0");
	let msgBuff = new ArrayBuffer(msg.length + HEADER_SIZE);
	let msgDataView = new DataView(msgBuff);

	msgDataView.setInt16(0, msg.length);
	msgDataView.setInt16(2, msg_type);

	for (var i = 0; i < msg.length; i++) {
		msgDataView.setUint8(i + HEADER_SIZE, msg.charCodeAt(i));
	}
	return msgBuff;
};
const parse_message = (msg) => {
	const view = new DataView(msg);
	const len = view.getInt16(0);
	const type = view.getInt16(2);
	const rawBody = String.fromCharCode.apply(
		null,
		Array.from(new Uint8Array(msg, HEADER_SIZE, len))
	);
	const body = rawBody.split("\0");

	return {
		type,
		body,
	};
};

let ws = new WebSocket("ws://192.168.100.117:81/");

ws.binaryType = "arraybuffer";

ws.onopen = () => {
	console.log("connected");
};

ws.onmessage = (e) => {
	// if (e.data instanceof ArrayBuffer) {
	// 	processMsg(e.data);
	// }
};

let state = 0;

document.getElementById("onoffbutton").addEventListener("click", () => {
	if (state == 0) {
		state = 1;
		document.getElementById("onoffbutton").innerHTML = "Turn Off";
	} else {
		state = 0;
		document.getElementById("onoffbutton").innerHTML = "Turn On";
	}
	ws.send(create_message(1, "2", state.toString()));
});
