var device_online = false
var socket = io.connect();
const tempo_ele = document.getElementById("tempoid");
const volume_ele = document.getElementById("volumeid");

socket.on('commandReply',(result) =>{
    if(!device_online){
        device_online = !device_online;
        $("#online_status")[0].style.color = "green";
        $("#online_status")[0].innerText = "online";
        $("#status")[0].hidden = false;
    }
    var parsed_data = result.split( " ");
    switch(parsed_data[0]){
        case "tempo":
            tempo_ele.value = parseInt(parsed_data[1]);
            break;
        case "volume":
            volume_ele.value = parseInt(parsed_data[1]);
            break;
        case "time":
            let time_val = parseInt(parsed_data[1]);
            document.getElementById("status_time").innerText = "device uptime:\n " + time_convert(time_val) + "  (H:M:S)";
            break;
    }
});

$("#tempoDown").click(()=>{
    if(device_online){
        var val = parseInt(tempo_ele.value);
        val = val - 5 > 40 ?  val - 5 : 40;
        socket.emit('command',"tempo " + val + "\n");
    }
});

$("#tempoUp").click(()=>{
    if(device_online){ 
        var val = parseInt(tempo_ele.value);
        val = val + 5 < 300 ?  val + 5 : 300;
        socket.emit('command',"tempo " + val + "\n");
    }
});

$("#volumeDown").click(()=>{
    if(device_online){
        var val = parseInt(volume_ele.value);
        val = val - 5 > 0 ?  val - 5 : 0;
        socket.emit('command',"volume " + val + "\n");
    }
});

$("#volumeUp").click(()=>{
    if(device_online){
        var val = parseInt(volume_ele.value);
        val = val + 5 < 100 ?  val + 5 : 100;
        socket.emit('command',"volume " + val + "\n");
    }
});


$("#modeNone").click(()=>{
    if(device_online){
        socket.emit('command',"mode 0\n");
    }
});

$("#modeRock1").click(()=>{
    if(device_online){
        socket.emit('command',"mode 1\n");
    }
});

$("#modeRock2").click(()=>{
    if(device_online){
        socket.emit('command',"mode 2\n");
    }
});

$("#hihat").click(()=>{
    if(device_online){
        socket.emit('command',"sound 0\n");
    }
});

$("#snare").click(()=>{
    if(device_online){
        socket.emit('command',"sound 1\n");
    }
});

$("#base").click(()=>{
    if(device_online){
        socket.emit('command',"sound 2\n");
    }
});

const time_convert = (seconds)=>{
    seconds = parseInt(seconds);
    let hour = seconds / 3600;
    seconds = seconds % 3600;
    let mins = seconds / 60;
    seconds = seconds % 60;
    return parseInt(hour) + " : " + parseInt(mins) + " : " + (seconds)
};

$( document ).ready(function() {
    window.setInterval(() =>{
        socket.emit('command',"uptime\n");
    },1000);
});

socket.emit('command',"tempo " + 120 + "\n");
socket.emit('command',"volume " + 80 + "\n");