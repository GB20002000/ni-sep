const toggles = document.querySelectorAll(".toggle");
const toogle1= document.getElementById("toogle1");
const toogle2= document.getElementById("toogle2");
const toogle3= document.getElementById("toogle3");
const toogle4= document.getElementById("toogle4");
const count=document.getElementById("count");
const ucount=document.getElementById("ucount");
const billcount=document.getElementById("billcount");
const sdt=document.getElementById("std");

const broker = "wss://53226e2c2b854db5b2ead0d6a5a8d1f3.s1.eu.hivemq.cloud:8884/mqtt"; 
const options = {
    clientId: "web_" +crypto.randomUUID(),
    username: "Check",  
    password: "123456789Gb@", 
    clean: true
};
const client = mqtt.connect(broker, options);
client.on("connect", async function () {
    console.log("Connected to HiveMQ Cloud");
    // client.subscribe("Led");
    client.subscribe("Switch");
})
client.on("message", (topic, message)=>{
    const payload =  message.toString();
    try {
        const str=decodeJson(payload)
        switch (topic){
            case 'Switch':
                Switch(str)
                break;
        }
    } catch (error) {
       console.log(payload)   
       console.error(error)   
    }
});
const decodeJson=(str)=>{
    try{
        return JSON.parse(str)
    }catch(error){
        return str
    }
}
const Switch=(message)=>{
     count.innerHTML=`${message.voltage} ${message.Vunit}`
}

document.addEventListener("DOMContentLoaded", function() {
    // toggles.forEach((toggle, index) => {
        toogle1.addEventListener("change", function() {
            // const roomStatus = toggle.closest(".controls").querySelector(".status");
            let state1 = toogle1.checked? "ON" : "OFF";
            if (state1=="ON") {
                // roomStatus.classList.remove("undetected");
                // roomStatus.classList.add("detected");
                // roomStatus.textContent = "PERSON DETECTED";
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led1",state1);
                console.log("state1",state1);
            
            } 
            else if(state1=="OFF") {
                // roomStatus.classList.remove("detected");
                // roomStatus.classList.add("undetected");
                // roomStatus.textContent = "PERSON UNDETECTED";
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led1",state1);
                console.log("state1",state1);
            }
            });

        toogle2.addEventListener("change", function() {
            let state2 = toogle2.checked? "ON" : "OFF";
            if (state2=="ON") {
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led2",state2);
                console.log("state2",state2);
                
            } 
            else if(state2=="OFF") {
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led2",state2);
                console.log("state2",state2);
                }
            });

        toogle3.addEventListener("change", function() {
            let state3 = toogle3.checked? "ON" : "OFF";
            if (state3=="ON") {
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led3",state3);
                console.log("state3",state3);
                    
            } 
            else if(state3=="OFF") {
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led3",state3);
                console.log("state3",state3);
                    }
            });

        toogle4.addEventListener("change", function() {
            let state4 = toogle4.checked? "ON" : "OFF";
            if (state4=="ON") {
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led2",state4);
                console.log("state4",state4);
            } 
            else if(state4=="OFF") {
                sdt.innerText="PERSON UNDETECTED";
                client.publish("Led4",state4);
                console.log("state4",state4);
            }
        });
        });
    // });
