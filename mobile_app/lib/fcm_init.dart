/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */

import 'package:firebase_messaging/firebase_messaging.dart';

/// Handles background messages when the app is terminated or in the background.
Future<void> _firebaseMessagingBackgroundHandler(RemoteMessage message) async {
  // If you need to initialize Firebase for background message handling
  // await Firebase.initializeApp(options: DefaultFirebaseOptions.currentPlatform);
  print("Handling a background message: ${message.messageId}");
  // Here you can process the message data
}

/// Initializes Firebase Messaging for foreground, background, and terminated states.
Future<void> setupFCM() async {
  // 1. Get the instance of Firebase Messaging
  final messaging = FirebaseMessaging.instance;

  // 2. Request permissions for iOS and web
  await messaging.requestPermission(
    alert: true,
    announcement: false,
    badge: true,
    carPlay: false,
    criticalAlert: false,
    provisional: false,
    sound: true,
  );

  // 3. Get the device token (which you just provided!)
  final token = await messaging.getToken();
  print("FCM Token (Restored): $token");

  // 4. Handle background messages
  FirebaseMessaging.onBackgroundMessage(_firebaseMessagingBackgroundHandler);

  // 5. Handle foreground messages
  FirebaseMessaging.onMessage.listen((RemoteMessage message) {
    print('Got a message whilst in the foreground!');
    print('Message data: ${message.data}');

    if (message.notification != null) {
      print('Message also contained a notification: ${message.notification!.title}');
      // TODO: Implement local notification display here for the user to see
    }
  });

  // 6. Handle notification taps when the app is in the background/terminated
  FirebaseMessaging.onMessageOpenedApp.listen((RemoteMessage message) {
    print('A new onMessageOpenedApp event was published!');
    // TODO: Navigate the user to a specific screen based on the message content
  });
}