const toggles = document.querySelectorAll(".toggle");
const count = document.getElementById("count");
const sdt = document.getElementById("std");

const broker = "wss://53226e2c2b854db5b2ead0d6a5a8d1f3.s1.eu.hivemq.cloud:8884/mqtt";
const options = {
    clientId: "web_" + crypto.randomUUID(),
    username: "Check",
    password: "123456789Gb@",
    clean: true
};

const client = mqtt.connect(broker, options);

client.on("connect", () => {
    console.log("Connected to HiveMQ Cloud");
    client.subscribe("Switch");
});

client.on("message", (topic, message) => {
    try {
        const data = JSON.parse(message.toString());
        if (topic === "Switch") {
            count.innerHTML = `${data.voltage} ${data.Vunit}`;
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
