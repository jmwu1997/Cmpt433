const port = 8088;

const dgram = require('dgram')
const express = require('express');
const app = express();
const server = require('http').Server(app);
var io = require('socket.io')(server);

var current_tempo_value = 120
var current_volume_value = 80

server.listen(port,()=>console.log('Start listenning on port ' + port));
//set static path
app.use(express.static('public'));
//home page
app.get('/', (req,res) => res.send('index.html'));
app.post('/init',(req,res) => res.send({
    tempo: current_tempo_value,
    volume: current_volume_value
}));
//connected
io.on('connection', (socket) => {
    socket.on('disconnect', function () {
        console.log('A user disconnected');
     });

    socket.on('command' , (data)=>{
        // Handle an incoming message over the UDP from the local application.
        // reference by Dr.Brian Fraser's "udp-server.js"
        let udp_port = 12345;
        let host = '192.168.7.2';
        let buffer = Buffer.alloc(data.length,data);

        var client = dgram.createSocket('udp4');
        client.send(buffer, 0, buffer.length, udp_port, host, (err, bytes) => {
		    if (err) 
		    	throw err;
		    console.log('UDP message sent to ' + host +':'+ udp_port + ", command is " + buffer );
        });
        
        client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
        });
        
        
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx: " + remote.address + ':' + remote.port +' - ' + message);
		    
		    var reply = message.toString('utf8')
		    socket.emit('commandReply', reply);
		    
		    client.close();

		});
		client.on("UDP Client: close", function() {
		    console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
		    console.log("error: ",err);
		});
    });
});