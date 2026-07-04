# TimecardApp — React Native

## Run on iOS (Xcode)

Open the workspace in Xcode (already opened for you):

```
ios/TimecardApp.xcworkspace
```

Select a simulator or a connected device, then hit **Run (⌘R)**.

You can also run from the terminal:

```bash
cd /Users/nostalgintosh/Documents/TimecardApp
npx react-native run-ios
```

## Run on Android

The Android Studio project is in the `android` folder. Open that folder in
Android Studio, wait for Gradle sync to finish, then select a connected phone or
emulator and press **Run**.

For a Galaxy Z Fold:

1. On the phone, enable **Developer options** and **USB debugging**.
2. Connect the phone to the Mac with a data-capable USB cable.
3. Accept the USB debugging prompt on the phone.
4. Select the Fold in Android Studio's device menu and press **Run**.

You can also run from Terminal after setting Android Studio's bundled Java
runtime:

```bash
cd /Users/nostalgintosh/Documents/TimecardApp
export JAVA_HOME="/Applications/Android Studio.app/Contents/jbr/Contents/Home"
export ANDROID_HOME="$HOME/Library/Android/sdk"
npx react-native run-android
```

## Enable Apple quick sign-in

The local simulator build intentionally keeps Apple quick sign-in disabled until
the Apple Developer configuration is complete. Opening Apple's sign-in sheet
before that setup can leave the simulator waiting indefinitely.

1. In Xcode, open `ios/TimecardApp.xcworkspace`.
2. Select the `TimecardApp` target, then open **Signing & Capabilities**.
3. Assign your Apple Developer team.
4. Replace `org.reactjs.native.example.TimecardApp` with a unique bundle ID.
5. Register that App ID in the Apple Developer portal and enable **Sign in with Apple**.
6. Set `APPLE_QUICK_SIGN_IN_CONFIGURED` to `true` in `src/appleQuickSignIn.ts`.
7. Run the signed build on an iPhone for final verification.

## Enable Google sign-in

This folder is pointed at Firebase project `timecardapp-fd7ef`. Google sign-in
requires real iOS/Android Firebase app config files plus OAuth clients from the
same Firebase project.

1. In the Firebase console, create or open the TimecardApp project.
2. Under **Authentication → Sign-in method**, enable **Google**.
3. Add the iOS app using bundle ID `org.reactjs.native.example.TimecardApp`.
4. Add the Android app using package name `com.timecardapp`.
5. Add the Android debug SHA fingerprints:
   - SHA-1: `5E:8F:16:06:2E:A3:CD:2C:4A:0D:54:78:76:BA:A6:F3:8C:AB:F6:25`
   - SHA-256: `FA:C6:17:45:DC:09:03:78:6F:B9:ED:E6:2A:96:2B:39:9F:73:48:F0:BB:6F:89:9B:83:32:66:75:91:03:3B:9C`
6. Download `GoogleService-Info.plist` for iOS and `google-services.json` for Android.
7. Run:

```bash
cd /Users/nostalgintosh/Documents/TimecardApp
./scripts/configure-firebase.sh ~/Downloads/GoogleService-Info.plist ~/Downloads/google-services.json
cd ios && pod install
```

The repair tool installs both downloads, extracts the Web OAuth client ID, and
adds the iOS callback URL scheme. If the downloaded files do not contain OAuth
clients, enable Google sign-in in Firebase Authentication, add the Android SHA
fingerprints above, then download the files again. Rebuild the app after the
repair tool finishes.

## App features

- **PUNCHING tab** — Clock in / Clock out, live shift hours, break timers (15 min / 30 min / 1 hour), today's earnings summary
- **HISTORY tab** — All past sessions grouped by date with daily totals
- **SETTINGS tab** — Hourly rate input, pay period (Weekly / Bi-Weekly / Semi-Monthly / Monthly / Yearly)
- **Profiles** — Create and delete multiple employee profiles; each profile stores its own rate, period, and history
- **Persistence** — All data stored with AsyncStorage (survives app restarts)
