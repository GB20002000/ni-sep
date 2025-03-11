document.addEventListener("DOMContentLoaded", function() {
    const toggles = document.querySelectorAll(".toggle");

    toggles.forEach((toggle, index) => {
        toggle.addEventListener("change", function() {
            const roomStatus = toggle.closest(".controls").querySelector(".status");

            if (toggle.checked) {
                roomStatus.classList.remove("undetected");
                roomStatus.classList.add("detected");
                roomStatus.textContent = "PERSON DETECTED";
            } else {
                roomStatus.classList.remove("detected");
                roomStatus.classList.add("undetected");
                roomStatus.textContent = "PERSON UNDETECTED";
            }
        });
    });
});
