const toggles = document.querySelectorAll(".toggle");
const count = document.getElementById("count");
const room1 = document.getElementById("room1");
const ucount =document.getElementById("ucount");
const billcount= document.getElementById("billcount");
const room2=document.getElementById("room2");

const broker = "wss://1fae4ab464e64fe9be19c16c1101c1be.s1.eu.hivemq.cloud:8884/mqtt";
const options = {
    clientId: "web_" + crypto.randomUUID(),
    username: "Check",
    password: "2025Black",
    clean: true
};

const client = mqtt.connect(broker, options);

client.on("connect", () => {
    console.log("Connected to HiveMQ Cloud");
    client.subscribe("Switch");
    client.subscribe("Switch2");
    client.subscribe("Switch3");
    client.subscribe("Alert1");
    client.subscribe("Alert2");
});
client.on("message", (topic, message) => {
    try {
        const data = JSON.parse(message.toString());
        const data1 =JSON.parse(message.toString());
        const data2 = JSON.parse(message.toString());
        const data3 = JSON.parse(message.toString());
        const data4 = JSON.parse(message.toString());
        if (topic === "Switch") {
            count.innerHTML = `${data.voltage} ${data.Vunit}`;
        }
        else if(topic ==="Switch2")
        {
            ucount.innerHTML = `${data1.voltage} ${data.Vunit}`;
        }
        else if(topic ==="Switch3")
        {
            billcount.innerHTML = `${data2.voltage} ${data.Vunit}`;
        }
        else if(topic ==="Alert1")
        {   
            if(data3.voltage ===1)
                {
                    room1.innerHTML="Person Dected";
                    room1.style.backgroundColor="blue";
                    alert("Person detected in Room1");
                }
                else if(data3.voltage ===0)
                    {
                        room1.innerHTML="Person UNDected";
                        room1.style.backgroundColor="red";
                    }

            
        }
        else if(topic ==="Alert2")
        {
            if(data4.voltage ===1)
                {
                    room2.innerHTML="Person Dected";
                    room2.style.backgroundColor="blue";
                    alert("Person detected in Room2");
                }
                else if(data4.voltage ===0)
                {
                    room2.innerHTML="Person UNDected";
                    room2.style.backgroundColor="red";
                }
        }

        
    } catch (error) {
        console.log("Received:", message.toString());
        console.error(error);
    }
});
document.addEventListener("DOMContentLoaded", function () {
    toggles.forEach((toggle, index) => {
        toggle.addEventListener("change", function () {
            const state = toggle.checked ? "ON" : "OFF";
            client.publish(`Led${index + 1}`, state);
            console.log(`Led${index + 1} State:`, state);
        });
    });
});
