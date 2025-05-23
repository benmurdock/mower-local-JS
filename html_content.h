// html_content.h
#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

//this is the html + css + js for user I/O
const String htmlPage = R"mydelimeter(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Mower Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 20px;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
        }
        h2 {
            color: #666;
            margin-bottom: 30px;
        }
        .button-container {
            display: flex;
            justify-content: center;
            gap: 20px;
            margin-bottom: 30px;
        }
        .control-group {
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        button {
            background-color: #007bff;
            color: white;
            padding: 12px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 18px;
            margin-bottom: 10px;
        }
        button:hover {
            background-color: #0056b3;
        }
        .value-display {
            font-size: 24px;
            font-weight: bold;
            color: #333;
        }
        #joystick-container {
            position: relative;
            width: 200px;
            height: 200px;
            background-color: #ddd;
            border-radius: 50%;
            margin: 40px auto;
            border: 2px solid #aaa;
            touch-action: none; /* Prevent scrolling on mobile */
        }
        #joystick-handle {
            position: absolute;
            width: 60px;
            height: 60px;
            background-color: #555;
            border-radius: 50%;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            cursor: grab;
            touch-action: none; /* Prevent scrolling on mobile */
        }
        .joystick-info {
            margin-top: 20px;
            font-size: 1.2em;
            color: #444;
        }
        .client-info-container {
            margin-top: 40px;
            padding: 20px;
            background-color: #e9e9e9;
            border-radius: 8px;
            display: inline-block; /* To center it */
            max-width: 90%;
            text-align: left;
        }
        .client-info-container h3 {
            text-align: center;
            margin-top: 0;
            color: #333;
        }
        .client-info-container ul {
            list-style-type: none;
            padding: 0;
        }
        .client-info-container li {
            margin-bottom: 5px;
            color: #555;
        }
    </style>
</head>
<body>
    <h1>Connected to ESP Mower</h1>
    <h2 id="uptime">Uptime: 0 sec</h2>

    <div class="button-container">
        <div class="control-group">
            <button id="groundSpeedBtn" onclick="toggleGroundSpeed()">Ground Speed: Slow</button>
            <div id="groundSpeedValue" class="value-display">100</div>
        </div>
        <div class="control-group">
            <button id="bladeBtn" onclick="toggleBlade()">Blade: off</button>
            <div id="bladeValue" class="value-display">0</div>
        </div>
    </div>

    <div id="joystick-container">
        <div id="joystick-handle"></div>
    </div>
    <div class="joystick-info">
        Polar: R: <span id="joystickR">0.00</span>, Angle: <span id="joystickAngle">0.00</span>&deg;
    </div>
    <div class="joystick-info">
        Left Track: <span id="leftTrackSpeed">0.00</span>, Right Track: <span id="rightTrackSpeed">0.00</span>
    </div>

    <div class="client-info-container">
        <h3>Connected Clients (<span id="clientCount">0</span>)</h3>
        <ul id="clientList">
            <li>No clients connected.</li>
        </ul>
    </div>


    <script>
        let groundSpeedState = 0; // 0 for Slow, 1 for Fast
        let bladeState = 0; // 0 for off, 1 for 5000, 2 for 12000

        function toggleGroundSpeed() {
            groundSpeedState = (groundSpeedState + 1) % 2;
            updateGroundSpeedButton();
            sendButtonState('groundSpeed', groundSpeedState);
        }

        function updateGroundSpeedButton() {
            const btn = document.getElementById('groundSpeedBtn');
            const val = document.getElementById('groundSpeedValue');
            if (groundSpeedState === 0) {
                btn.textContent = 'Ground Speed: Slow';
                val.textContent = '100';
            } else {
                btn.textContent = 'Ground Speed: Fast';
                val.textContent = '200';
            }
        }

        function toggleBlade() {
            bladeState = (bladeState + 1) % 3;
            updateBladeButton();
            sendButtonState('blade', bladeState);
        }

        function updateBladeButton() {
            const btn = document.getElementById('bladeBtn');
            const val = document.getElementById('bladeValue');
            if (bladeState === 0) {
                btn.textContent = 'Blade: off';
                val.textContent = '0';
            } else if (bladeState === 1) {
                btn.textContent = 'Blade: 5000 fpm';
                val.textContent = '5000';
            } else {
                btn.textContent = 'Blade: 12000 fpm';
                val.textContent = '12000';
            }
        }

        function sendButtonState(type, state) {
            const xhr = new XMLHttpRequest();
            xhr.open('GET', '/' + type + '?state=' + state, true);
            xhr.send();
        }

        // Uptime update
        setInterval(() => {
            fetch('/uptime')
                .then(response => response.text())
                .then(uptime => {
                    document.getElementById('uptime').textContent = 'Uptime: ' + uptime + ' sec';
                });
        }, 1000);

        // Joystick functionality
        const joystickContainer = document.getElementById('joystick-container');
        const joystickHandle = document.getElementById('joystick-handle');
        const joystickR = document.getElementById('joystickR');
        const joystickAngle = document.getElementById('joystickAngle');
        const leftTrackSpeedDisplay = document.getElementById('leftTrackSpeed');
        const rightTrackSpeedDisplay = document.getElementById('rightTrackSpeed');

        let isDragging = false;
        // Get container dimensions relative to the viewport
        let containerRect = joystickContainer.getBoundingClientRect();
        let containerCenterX = containerRect.width / 2;
        let containerCenterY = containerRect.height / 2;
        // Max radius for joystick handle movement
        let maxRadius = containerRect.width / 2 - joystickHandle.offsetWidth / 2;

        // Recalculate dimensions on window resize (important for responsive design)
        window.addEventListener('resize', () => {
            containerRect = joystickContainer.getBoundingClientRect();
            containerCenterX = containerRect.width / 2;
            containerCenterY = containerRect.height / 2;
            maxRadius = containerRect.width / 2 - joystickHandle.offsetWidth / 2;
        });


        joystickHandle.addEventListener('touchstart', (e) => {
            isDragging = true;
            joystickHandle.style.cursor = 'grabbing';
            e.preventDefault(); // Prevent scrolling
        });

        joystickContainer.addEventListener('touchmove', (e) => {
            if (!isDragging) return;

            const touch = e.touches[0];
            // Calculate coordinates relative to the center of the joystick container
            let x = touch.clientX - containerRect.left - containerCenterX;
            let y = touch.clientY - containerRect.top - containerCenterY;

            // Clamp joystick within the circle
            const distance = Math.sqrt(x * x + y * y);
            if (distance > maxRadius) {
                x = (x / distance) * maxRadius;
                y = (y / distance) * maxRadius;
            }

            // Position the handle
            joystickHandle.style.left = (containerCenterX + x) + 'px';
            joystickHandle.style.top = (containerCenterY + y) + 'px';

            updateJoystickOutput(x, y);
            e.preventDefault();
        });

        joystickHandle.addEventListener('touchend', () => {
            isDragging = false;
            joystickHandle.style.cursor = 'grab';
            // Snap back to center
            joystickHandle.style.left = '50%';
            joystickHandle.style.top = '50%';
            updateJoystickOutput(0, 0); // Reset values
        });

        function updateJoystickOutput(x, y) {
            // Normalize x and y to be between -1 and 1 based on maxRadius
            const normalizedX = x / maxRadius;
            const normalizedY = -y / maxRadius; // Y-axis inverted for standard math (up is positive)

            // Calculate polar coordinates
            let r = Math.sqrt(normalizedX * normalizedX + normalizedY * normalizedY);
            let angle = Math.atan2(normalizedY, normalizedX) * (180 / Math.PI); // Convert radians to degrees
            if (angle < 0) angle += 360; // Ensure angle is 0-360 (0-360 degrees)

            // Apply dead zone for r
            const deadZone = 0.1; // Adjust as needed
            if (r < deadZone) {
                r = 0;
            } else {
                r = (r - deadZone) / (1 - deadZone); // Scale r to 0-1 after dead zone
            }

            joystickR.textContent = r.toFixed(2);
            joystickAngle.textContent = angle.toFixed(2);

            // Convert to bulldozer differential steering
            // forwardSpeed is the magnitude along the Y-axis (forward/backward)
            // turnSpeed is the magnitude along the X-axis (left/right turning)
            let forwardSpeed = r * Math.cos(angle * Math.PI / 180); // Y-axis for forward/backward
            let turnSpeed = r * Math.sin(angle * Math.PI / 180);    // X-axis for turning

            // Differential steering calculation for a bulldozer/tank:
            // This is a common simplified model.
            // When moving forward, a positive turnSpeed adds to one track and subtracts from the other.
            // When moving backward, the roles are reversed (or it behaves similarly depending on desired control).
            let leftTrack = forwardSpeed + turnSpeed;
            let rightTrack = forwardSpeed - turnSpeed;

            // Clamp speeds to -1.0 for full speed backwards to 1.0 for full speed forwards
            leftTrack = Math.max(-1.0, Math.min(1.0, leftTrack));
            rightTrack = Math.max(-1.0, Math.min(1.0, rightTrack));

            leftTrackSpeedDisplay.textContent = leftTrack.toFixed(2);
            rightTrackSpeedDisplay.textContent = rightTrack.toFixed(2);

            // Send joystick data to ESP32 via AJAX
            sendJoystickData(r, angle, leftTrack, rightTrack);
        }

        function sendJoystickData(r, angle, leftTrack, rightTrack) {
            const xhr = new XMLHttpRequest();
            xhr.open('GET', '/joystick?r=' + r.toFixed(2) + '&angle=' + angle.toFixed(2) + '&left=' + leftTrack.toFixed(2) + '&right=' + rightTrack.toFixed(2), true);
            xhr.send();
        }

        // Client Info Update
        setInterval(() => {
            fetch('/clients')
                .then(response => response.json()) // Expect JSON response
                .then(data => {
                    document.getElementById('clientCount').textContent = data.numClients;
                    const clientList = document.getElementById('clientList');
                    clientList.innerHTML = ''; // Clear existing list

                    if (data.numClients === 0) {
                        clientList.innerHTML = '<li>No clients connected.</li>';
                    } else {
                        data.clients.forEach(client => {
                            const li = document.createElement('li');
                            li.textContent = 'MAC: ' + client.mac + ', RSSI: ' + client.rssi + ' dBm';
                            clientList.appendChild(li);
                        });
                    }
                })
                .catch(error => {
                    console.error('Error fetching client info:', error);
                    document.getElementById('clientList').innerHTML = '<li>Error loading client info.</li>';
                });
        }, 3000); // Update client info every 3 seconds

        // Initial button states
        updateGroundSpeedButton();
        updateBladeButton();

    </script>
</body>
</html>
)mydelimeter";

#endif