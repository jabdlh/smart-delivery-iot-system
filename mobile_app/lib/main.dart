import 'dart:convert';
import 'dart:typed_data';


import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

// The PushNotificationManager class, all FCM background handlers,
// and local notification imports have been removed.

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  
  // Initialize Firebase Core (kept for DefaultFirebaseOptions/initialization, 
  // but notification-related services are no longer used)
  
  
  
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  // FCM Token state and PushNotificationManager initialization have been removed.

  // --- NEW STATE VARIABLE FOR ACCEPT/RETRIEVE LOGIC ---
  // If true, ACCEPT is disabled and RETRIEVE is enabled.
  bool _isAcceptedAndLocked = false;

  // MQTT Client Setup
  final client = MqttServerClient(
    '613c0e519ce74137888ff912eb7880d8.s1.eu.hivemq.cloud',
    'flutter_${DateTime.now().millisecondsSinceEpoch}',
  );

  String deviceStatus = 'Awaiting Status...';
  String status = 'Connecting...';
  String temperature = '--';
  String weightReading = '--';

  String imageBuffer = "";
  Uint8List? latestImage;
  bool imageDecodeError = false;

  Map<String, bool> commandStatus = {
    'ACCEPT': false,
    'REJECT': false,
    'RETRIEVE': false,
    'OPEN DOOR': false,
    'CLOSE DOOR': false,
    'HEATER ON': false,
    'HEATER OFF': false,
  };

  // Log list to store command entries
  List<String> commandLog = [];

  @override
  void initState() {
    super.initState();
    // Notification initialization call has been removed.
    _connect(); // Initial connection attempt
  }

  // --- MQTT Connection Logic ---
  Future<void> _connect() async {
    setState(() => status = 'Connecting...');
    client.port = 8883;
    client.secure = true;
    client.keepAlivePeriod = 20;
    client.logging(on: true);

    client.onConnected = () => setState(() => status = 'Connected');
    client.onDisconnected = () {
      setState(() => status = 'Disconnected. Retrying in 5s...');

      // Local notification trigger on disconnection removed.
      
      // Auto-reconnect logic
      Future.delayed(const Duration(seconds: 5), _connect); 
    };
    client.onSubscribed = (topic) => print('Subscribed to $topic');
    
    // Set up connection message with credentials
    final connMessage = MqttConnectMessage()
        .withClientIdentifier(client.clientIdentifier ?? 'flutter_${DateTime.now().millisecondsSinceEpoch}')
        .authenticateAs('hivemq.webclient.1758400994140', 'N3jYnAocX4;BbC9*.\$a5')
        .startClean();

    client.connectionMessage = connMessage;

    try {
      await client.connect();
      if (client.connectionStatus?.state == MqttConnectionState.connected) {
        // Successful connection, subscribe to topics
        client.subscribe('esp32/temperature', MqttQos.atMostOnce);
        client.subscribe('esp32/image', MqttQos.atMostOnce);
        
        client.subscribe('esp32/weight', MqttQos.atMostOnce);
        client.subscribe('esp32/status', MqttQos.atMostOnce);
        client.updates?.listen(_onMessage);
      } else {
        // Connection failed (e.g., authentication error)
        setState(() => status = 'Connection failed: ${client.connectionStatus?.returnCode.toString() ?? 'Unknown Error'}');
        client.disconnect();
      }
    } catch (e) {
      // General exception (e.g., network error)
      setState(() => status = 'Network Error: $e');
      client.disconnect();
    }
  }

  Uint8List _payloadToUint8List(dynamic payloadMessage) {
    if (payloadMessage is Uint8List) return payloadMessage;
    if (payloadMessage is List<int>) return Uint8List.fromList(payloadMessage);
    try {
      final iter = payloadMessage as Iterable<int>;
      return Uint8List.fromList(iter.toList());
    } catch (_) {
      return Uint8List(0);
    }
  }

  void _onMessage(List<MqttReceivedMessage<MqttMessage?>> messages) {
    final recTopic = messages[0].topic;
    final recMess = messages[0].payload as MqttPublishMessage;
    final payloadBytes = _payloadToUint8List(recMess.payload.message);

    if (recTopic == 'esp32/image') {
      final chunk = String.fromCharCodes(payloadBytes);
      if (chunk == "EOF") {
        if (imageBuffer.isEmpty) return;
        try {
          // Base64 padding required if length is not a multiple of 4
          final padded = imageBuffer + '=' * ((4 - imageBuffer.length % 4) % 4);
          final bytes = base64Decode(padded);
          setState(() {
            latestImage = bytes;
            imageBuffer = "";
            imageDecodeError = false;
          });
        } catch (e) {
          setState(() {
            latestImage = null;
            imageBuffer = "";
            imageDecodeError = true;
          });
        }
        return;
      }
      imageBuffer += chunk;
      return;
    }

    final text = utf8.decode(payloadBytes);
    if (recTopic == 'esp32/temperature') {
      setState(() => temperature = text);
    }
    else if (recTopic == 'esp32/weight') {
      setState(() => weightReading = text);
    }
    else if (recTopic == 'esp32/status') {
        // Save the old status before updating
        // final oldStatus = deviceStatus; // Unused after notification removal
        setState(() => deviceStatus = text);

        // Notification Trigger Logic for ONLINE/OFFLINE removed.
    }
  }

  void _sendCommand(String command) {
    if (client.connectionStatus?.state == MqttConnectionState.connected) {
      final builder = MqttClientPayloadBuilder()..addString(command);
      client.publishMessage('esp32/control', MqttQos.atMostOnce, builder.payload!);

      setState(() {
        // Reset all command states and set the current one to true
        commandStatus.updateAll((key, value) => false);
        if (commandStatus.containsKey(command)) {
          commandStatus[command] = true;
        }
        // Log the command (Only happens if connected and sent)
        commandLog.add('${DateTime.now().toLocal().toString().substring(0, 19)} — $command');
      });

      // Immediate Command Confirmation Notification removed.
      
    } else {
      // If not connected, we don't send the command or add to the log.
      setState(() => status = 'Error: Not connected. Please check connection.');
      
      // Notify user the command failed due to connection removed.
    }
  }

  // The _triggerTestNotification method has been removed.

  @override
  void dispose() {
    client.disconnect();
    super.dispose();
  }

  ButtonStyle commonButtonStyle(Color color) {
    return ElevatedButton.styleFrom(
      backgroundColor: color,
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 16),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
      foregroundColor: Colors.white, // Ensure text is white
    );
  }

  // Updated buildStatusButton to accept an optional onPressed, allowing it to be disabled
  Widget buildStatusButton(String label, Color color, {VoidCallback? onPressed}) {
    return Column( 
      mainAxisSize: MainAxisSize.min,
      children: [
        ElevatedButton(
          style: commonButtonStyle(color),
          onPressed: onPressed, // Use the provided callback (null for disabled)
          child: Text(label, style: const TextStyle(fontSize: 12)),
        ),
        const SizedBox(height: 4),
        Chip(
          label: Text(commandStatus[label]! ? 'ACTIVE' : 'INACTIVE'),
          backgroundColor: commandStatus[label]! ? Colors.greenAccent : Colors.grey[300],
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    // Get the last 5 log entries, or fewer if the log is shorter.
    final recentLogEntries = commandLog.reversed.take(5).toList();

    // FIX: Safe weight parsing and display formatting
    final safeWeight = double.tryParse(weightReading);
    final weightDisplay = safeWeight != null ? safeWeight.toStringAsFixed(3) : weightReading;

    // Determine colors based on the locked state
    final Color acceptColor = _isAcceptedAndLocked ? Colors.grey : Colors.green;
    final Color retrieveColor = _isAcceptedAndLocked ? Colors.blue : Colors.grey;

    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          // FIX 1: Wrap title in FittedBox to prevent truncation on smaller screens
          title: const FittedBox(
            child: Text('KKU Smart Multi-Compartment System'),
          ),
          backgroundColor: Colors.blueGrey,
          foregroundColor: Colors.white,
        ),
        body: SafeArea(
          child: SingleChildScrollView(
            padding: const EdgeInsets.all(20),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                // =======================================================
                // STATUS BAR
                // =======================================================
                Container(
                  color: Colors.grey[100], // Subtle background for visual separation
                  padding: const EdgeInsets.symmetric(vertical: 8.0),
                  child: SingleChildScrollView(
                    scrollDirection: Axis.horizontal,
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.center, 
                      children: [
                        const SizedBox(width: 20),

                        // --- 1. MQTT Connection Status (Grouped for Alignment) ---
                        Row(
                          mainAxisSize: MainAxisSize.min, 
                          children: [
                            Text(status, style: const TextStyle(fontSize: 14)),
                            const SizedBox(width: 8),
                            // Status Indicator Light
                            CircleAvatar(
                              radius: 6,
                              backgroundColor: status == 'Connected' ? Colors.green : Colors.red,
                            ),
                          ],
                        ),
                        
                        const SizedBox(width: 24), 
                        
                        // --- 2. RECONNECT Button / ESP32 Status Column (Vertical Grouping) ---
                        Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            // RECONNECT Button
                            ElevatedButton.icon(
                              onPressed: _connect,
                              icon: const Icon(Icons.refresh, size: 16),
                              label: const Text('RECONNECT', style: TextStyle(fontSize: 12)),
                              style: ElevatedButton.styleFrom(
                                backgroundColor: Colors.amber[700],
                                foregroundColor: Colors.white,
                                padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                              ),
                            ),
                            
                            const SizedBox(height: 8), 

                            // ESP32 Device Status
                            Row(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                const Text('ESP32 Status: ', style: TextStyle(fontSize: 14)),
                                Text(
                                    deviceStatus, 
                                    style: TextStyle(
                                      fontSize: 14, 
                                      fontWeight: FontWeight.bold,
                                      color: deviceStatus == 'ONLINE' ? Colors.green[800] : Colors.red[800]
                                    )
                                ),
                              ],
                            ),
                          ],
                        ),
                        
                        const SizedBox(width: 20), 
                      ],
                    ),
                  ),
                ),
                // =======================================================
                // END OF STATUS BAR SECTION
                // =======================================================

                const SizedBox(height: 20),
                
                // --- Temperature Card ---
                Card(
                  elevation: 4,
                  child: Padding(
                    padding: const EdgeInsets.all(20),
                    child: Text('Temperature: $temperature °C', style: const TextStyle(fontSize: 20)),
                  ),
                ),
                
                // --- Weight Card ---
                Card(
                  elevation: 4,
                  child: Padding(
                    padding: const EdgeInsets.all(20),
                    child: Text('Current Weight: $weightDisplay g', style: const TextStyle(fontSize: 20)),
                  ),
                ),

                  // Image Display Card
                Card(
                  elevation: 4,
                  child: Padding(
                    padding: const EdgeInsets.all(12),
                    child: Column(
                      children: [
                        const Text('Latest Capture', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold)),
                        const SizedBox(height: 10),
                        Container(
                          width: double.infinity,
                          constraints: const BoxConstraints(maxHeight: 400),
                          color: Colors.black12,
                          child: AspectRatio(
                            aspectRatio: 4 / 3, // Common camera aspect ratio
                            child: Center(
                              child: Builder(builder: (context) {
                                if (latestImage != null) {
                                  return Image.memory(
                                    latestImage!,
                                    fit: BoxFit.contain,
                                  );
                                } else if (imageDecodeError) {
                                  return const Text(
                                    'Decode error; buffer cleared',
                                    style: TextStyle(color: Colors.red),
                                  );
                                } else {
                                  return const Column(
                                    mainAxisAlignment: MainAxisAlignment.center,
                                    children: [
                                      CircularProgressIndicator(),
                                      SizedBox(height: 10),
                                      Text('Waiting for image...', style: TextStyle(color: Colors.grey)),
                                    ],
                                  );
                                }
                              }),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
                
                const SizedBox(height: 24),
                
                // Capture Button
                Center(
                  child: ElevatedButton.icon(
                    style: ElevatedButton.styleFrom(
                      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 15),
                      backgroundColor: const Color.fromARGB(255, 91, 57, 199),
                      foregroundColor: Colors.white,
                    ),
                    onPressed: () => _sendCommand('CAPTURE'),
                    icon: const Icon(Icons.camera_alt, size: 28),
                    label: const Text('Capture Image', style: TextStyle(fontSize: 16)),
                  ),
                ),
                
                const SizedBox(height: 12),
                
                // Door Controls
                ExpansionTile(title: const Text('Door Controls'), children: [
                  buildStatusButton('OPEN DOOR', Colors.green, onPressed: () => _sendCommand('OPEN DOOR')),
                  buildStatusButton('CLOSE DOOR', Colors.orange, onPressed: () => _sendCommand('CLOSE DOOR')),
                ]),
                
                
                
                // Heater Controls
                ExpansionTile(title: const Text('Heater Controls'), children: [
                  buildStatusButton('HEATER ON', Colors.lightGreen, onPressed: () => _sendCommand('HEATER ON')),
                  buildStatusButton('HEATER OFF', Colors.red, onPressed: () => _sendCommand('HEATER OFF')),
                ]),
                
                // Sorting Commands (Includes RETRIEVE) - Now with State Control
                ExpansionTile(
                  title: Text('Sorting Commands (${_isAcceptedAndLocked ? "LOCKED" : "READY"})'), 
                  children: [
                    Padding(
                      padding: const EdgeInsets.symmetric(vertical: 8.0),
                      child: Wrap(
                        alignment: WrapAlignment.spaceEvenly,
                        spacing: 16.0, 
                        runSpacing: 16.0, 
                        children: [
                          // ACCEPT Button Logic
                          buildStatusButton(
                            'ACCEPT', 
                            acceptColor,
                            onPressed: _isAcceptedAndLocked 
                                ? null // Disabled if locked
                                : () {
                                    // Send Command and lock state
                                    _sendCommand('ACCEPT');
                                    setState(() {
                                        _isAcceptedAndLocked = true;
                                    });
                                  }
                          ),
                          
                          // REJECT Button (Always enabled, assuming REJECT is independent)
                          buildStatusButton('REJECT', Colors.red, onPressed: () => _sendCommand('REJECT')),

                          // RETRIEVE Button Logic
                          buildStatusButton(
                            'RETRIEVE', 
                            retrieveColor,
                            onPressed: _isAcceptedAndLocked 
                                ? () {
                                    // Send Command and unlock state
                                    _sendCommand('RETRIEVE');
                                    setState(() {
                                        _isAcceptedAndLocked = false;
                                    });
                                  } 
                                : null // Disabled if not locked
                          ),
                        ],
                      ),
                    ),
                  ],
                ),
                
                const SizedBox(height: 20),
                
                // View Log Button (Using Builder for robust context)
                Builder(
                  builder: (innerContext) => ElevatedButton.icon(
                    style: ElevatedButton.styleFrom(
                      backgroundColor: Colors.blueGrey,
                      foregroundColor: Colors.white,
                      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 14),
                      shape: RoundedRectangleBorder( borderRadius: BorderRadius.circular(8)),
                    ),
                    onPressed: () {
                      // Use the innerContext from the Builder for navigation
                      Navigator.push(
                        innerContext,
                        MaterialPageRoute(
                          builder: (context) => StatusPage(log: commandLog),
                        ),
                      );
                    },
                    icon: const Icon(Icons.list_alt),
                    label: const Text('View Full Status Log', style: TextStyle(fontSize: 14)),
                  ),
                ),

                const SizedBox(height: 30),

                // --- Immediate Log Feedback ---
                const Text(
                  'Recent Commands (Last 5)', 
                  style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
                ),
                const Divider(),

                if (recentLogEntries.isEmpty)
                  const Text('No commands logged yet. Connect and send a command.', style: TextStyle(color: Colors.grey))
                else
                  ...recentLogEntries.map((entry) => Padding(
                    padding: const EdgeInsets.symmetric(vertical: 4.0),
                    child: Text(entry, style: const TextStyle(fontFamily: 'monospace', fontSize: 13)),
                  )),
                
                const SizedBox(height: 30),
              
              ],
            ),
          ),
        ),
      ),
    );
  }
}

class StatusPage extends StatelessWidget {
  final List<String> log;
  const StatusPage({super.key, required this.log});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Command Log'),
        backgroundColor: Colors.blueGrey,
        foregroundColor: Colors.white,
      ),
      body: log.isEmpty
          ? const Center(child: Text('No commands sent yet'))
          : ListView.builder(
              padding: const EdgeInsets.all(16),
              itemCount: log.length,
              itemBuilder: (context, index) {
                // Display the log from newest to oldest
                final reversedIndex = log.length - 1 - index;
                return Card(
                  elevation: 2,
                  margin: const EdgeInsets.symmetric(vertical: 6),
                  child: Padding(
                    padding: const EdgeInsets.all(12),
                    child: Text(
                      log[reversedIndex],
                      style: const TextStyle(fontSize: 14),
                    ),
                  ),
                );
              },
            ),
    );
  }
}