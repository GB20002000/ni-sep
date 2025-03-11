const toggles = document.querySelectorAll(".toggle");
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
    // client.subscribe("Switch");
})

document.addEventListener("DOMContentLoaded", function() {
    toggles.forEach((toggle, index) => {
        toggle.addEventListener("change", function() {
            const roomStatus = toggle.closest(".controls").querySelector(".status");
            let state = toggle.checked? "ON" : "OFF";
            if (state=="ON") {
                roomStatus.classList.remove("undetected");
                roomStatus.classList.add("detected");
                roomStatus.textContent = "PERSON DETECTED";
                client.publish("Led",state);
                console.log("state",state);
            
            } 
            else if(state=="OFF") {
                roomStatus.classList.remove("detected");
                roomStatus.classList.add("undetected");
                roomStatus.textContent = "PERSON UNDETECTED";
                client.publish("Led",state);
                console.log("state",state);
            }
            });
        });
    });
