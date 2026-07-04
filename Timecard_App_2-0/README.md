# Numberfile CheatSheet

Also called the **NFCS Protocol**.

This file is the map for finding code faster in TimecardApp. Use the NFCS number, file name, or search term to jump to the right part of the app.

## NFCS Quick Search

From the TimecardApp folder, search for a code, screen, function, or word:

```sh
rg "NF-200"
rg "handleClockIn"
rg "FirebaseAuthScreen"
rg "ScanToPunch"
```

If you are using an editor, press `Command + P` to open a file by name, or `Command + Shift + F` to search the whole project.

## Numberfile Index

| NFCS code | Area | Main file | What it controls |
| --- | --- | --- | --- |
| NF-100 | App start | `App.tsx` | Main app, auth gate, tabs, timecard screen |
| NF-110 | Fold layout | `App.tsx` | Galaxy Z Fold / wide-screen layout decisions |
| NF-120 | Paper style | `App.tsx`, `src/FirebaseAuthScreen.tsx` | Ruled paper look, colors, spacing, typography |
| NF-200 | Profiles | `App.tsx` | Create, select, delete, and store profiles |
| NF-210 | Account storage | `App.tsx` | AsyncStorage keys for account-scoped profile data |
| NF-220 | Profile migration | `App.tsx` | Moves old profile data into account-scoped storage |
| NF-300 | Punch clock | `App.tsx` | Clock in, clock out, live hours, pay totals |
| NF-310 | Break timers | `App.tsx` | 15 min, 30 min, 1 hour breaks and paid-break settings |
| NF-320 | Pay periods | `App.tsx` | Weekly, bi-weekly, semi-monthly, monthly, yearly grouping |
| NF-400 | Schedule | `App.tsx` | Static, pod, rotating, and flex schedules |
| NF-410 | Calendar | `App.tsx` | Month calendar and schedule day editing |
| NF-500 | Firebase sign-in | `src/FirebaseAuthScreen.tsx`, `src/firebaseAuth.ts` | Email, Google, Apple, password reset, Firebase auth |
| NF-510 | Local auth legacy | `src/AuthScreen.tsx`, `src/auth.ts` | Older local account system, password hashing, biometrics |
| NF-520 | Apple quick sign-in flag | `src/appleQuickSignIn.ts` | Enables or disables Apple quick sign-in helper |
| NF-600 | Scan to punch | `src/ScanToPunch.tsx` | Camera/photo OCR import for punch times |
| NF-700 | App config | `app.json`, `firebase.json`, `package.json` | App name, scripts, Firebase settings, dependencies |
| NF-800 | iOS native files | `ios/` | Xcode workspace, plist, entitlements, CocoaPods |
| NF-810 | Android native files | `android/` | Android package, Gradle, manifest, launcher icons |
| NF-820 | Native theme cross-reference | `ios/`, `android/` | Xcode and Android Studio theme files side by side |
| NF-830 | Launch and icon theme | `ios/TimecardApp/LaunchScreen.storyboard`, `android/app/src/main/res/` | Splash screen, launcher icon, app startup visuals |
| NF-900 | Tests | `__tests__/` | Jest tests for app, auth, and screens |
| NF-950 | Firebase setup helper | `scripts/configure-firebase.sh` | Installs Firebase config files and callback setup |

## Main Code Map

### NF-100: App Start

File: `App.tsx`

Search terms:

```sh
rg "export default function AppRoot"
rg "function AuthGate"
rg "function App"
rg "activeTab"
```

Important spots:

| Line area | Name | Purpose |
| --- | --- | --- |
| Near top | `useFoldState` | Detects folded vs unfolded screen width |
| `AppRoot` | Root wrapper | Adds safe-area provider |
| `AuthGate` | Login gate | Decides whether user sees auth or the app |
| `App` | Main app | Owns profiles, clock state, schedules, tabs, and modals |

### NF-200: Profiles and Storage

File: `App.tsx`

Search terms:

```sh
rg "Profile"
rg "profilesKey"
rg "createProfile"
rg "confirmDelete"
```

Important functions and state:

| Name | Purpose |
| --- | --- |
| `Profile` | Basic profile type with id, name, and creation date |
| `ProfileData` | Saved data for rate, sessions, schedule, clock state, and settings |
| `profilesKey` | Builds the saved profile-list key for an account |
| `currentProfileKey` | Builds the saved current-profile key |
| `profileDataKey` | Builds the saved data key for one profile |
| `migrateLegacyProfileData` | Moves old profile data into the newer account-scoped format |
| `createProfile` | Adds a profile |
| `requestDelete` | Opens the delete confirmation |
| `confirmDelete` | Deletes the selected profile |

### NF-300: Punch Clock

File: `App.tsx`

Search terms:

```sh
rg "handleClockIn"
rg "handleClockOut"
rg "shiftHours"
rg "todayPay"
```

Important functions:

| Name | Purpose |
| --- | --- |
| `handleClockIn` | Starts a work session |
| `handleClockOut` | Ends a work session and saves it |
| `shiftHours` | Calculates live hours for the current shift |
| `todayHours` | Calculates total hours for today |
| `todayPay` | Calculates today's pay from hours and rate |
| `periodHours` | Calculates hours for the current pay period |
| `periodPay` | Calculates pay for the current pay period |

### NF-310: Break Timers

File: `App.tsx`

Search terms:

```sh
rg "startBreak"
rg "endBreak"
rg "paidBreaks"
rg "breakRecords"
```

Important functions and state:

| Name | Purpose |
| --- | --- |
| `startBreak` | Starts a timed break |
| `endBreak` | Ends the active break early or when time runs out |
| `paidBreaks` | Decides whether breaks reduce worked hours |
| `breakRecordsRef` | Tracks break history during the current shift |

### NF-320: Pay Periods

File: `App.tsx`

Search terms:

```sh
rg "payPeriodKey"
rg "payPeriodLabel"
rg "nextPayday"
rg "payPeriodFolders"
```

Important functions:

| Name | Purpose |
| --- | --- |
| `payPeriodKey` | Groups a work date into a pay-period bucket |
| `payPeriodLabel` | Makes a readable label for a pay-period bucket |
| `isCurrentPeriod` | Checks whether a bucket is the current pay period |
| `nextPayday` | Calculates the next payday display text |
| `payPeriodFolders` | Groups history by pay period |

### NF-400: Schedule System

File: `App.tsx`

Search terms:

```sh
rg "ScheduleConfig"
rg "DEFAULT_SCHED_CFG"
rg "scheduledStartFor"
rg "addPod"
rg "updateFlexDay"
```

Important pieces:

| Name | Purpose |
| --- | --- |
| `ScheduleConfig` | Main schedule settings type |
| `WeekSchedule` | Fixed weekly schedule |
| `PodDef` | Rotating pod schedule block |
| `FlexSchedule` | Date-by-date flexible schedule |
| `scheduledStartFor` | Finds the expected start time for a date |
| `lateMinutes` | Calculates whether the current clock-in is late |
| `addPod`, `updatePod`, `removePod` | Manage rotating pods |
| `updateFlexDay` | Edits one date in a flexible schedule |

### NF-500: Firebase Auth

Files:

- `src/FirebaseAuthScreen.tsx`
- `src/firebaseAuth.ts`

Search terms:

```sh
rg "signInWithEmail"
rg "createAccountWithEmail"
rg "signInWithGoogle"
rg "signInWithApple"
rg "friendlyError"
```

Important pieces:

| File | Name | Purpose |
| --- | --- | --- |
| `src/FirebaseAuthScreen.tsx` | `FirebaseAuthScreen` | Visible sign-in, sign-up, and password reset screen |
| `src/FirebaseAuthScreen.tsx` | `friendlyError` | Turns Firebase errors into readable messages |
| `src/firebaseAuth.ts` | `configureGoogleSignIn` | Sets up Google sign-in |
| `src/firebaseAuth.ts` | `onAuthStateChanged` | Watches whether the user is signed in |
| `src/firebaseAuth.ts` | `signInWithEmail` | Email/password sign-in |
| `src/firebaseAuth.ts` | `createAccountWithEmail` | Email/password account creation |
| `src/firebaseAuth.ts` | `sendPasswordReset` | Sends reset email |
| `src/firebaseAuth.ts` | `signInWithGoogle` | Google sign-in |
| `src/firebaseAuth.ts` | `signInWithApple` | Apple sign-in |
| `src/firebaseAuth.ts` | `signOut` | Signs out of Firebase |

### NF-510: Local Auth Legacy

Files:

- `src/AuthScreen.tsx`
- `src/auth.ts`

Search terms:

```sh
rg "createAccount" src/auth.ts
rg "resetPasswordWithCode"
rg "setBiometricsEnabled"
rg "ReactNativeBiometrics"
```

This older local-auth path stores accounts in AsyncStorage and supports recovery codes and biometrics. Firebase is the current main sign-in path, but these files are still useful if you need to inspect or reuse local account behavior.

### NF-600: Scan To Punch

File: `src/ScanToPunch.tsx`

Search terms:

```sh
rg "parseOcrText"
rg "openCamera"
rg "openLibrary"
rg "handleConfirm"
```

Important pieces:

| Name | Purpose |
| --- | --- |
| `parseTime` | Turns OCR text into a time |
| `parseBreakMins` | Detects 15, 30, or 60 minute breaks |
| `parseOcrText` | Reads OCR lines and finds punch times |
| `openCamera` | Starts camera image capture |
| `openLibrary` | Lets the user pick a photo |
| `handleConfirm` | Sends selected scanned punches back to the main app |

## File Tree CheatSheet

```text
TimecardApp/
  App.tsx                         NF-100 to NF-400 main app
  src/
    FirebaseAuthScreen.tsx        NF-500 visible Firebase login screen
    firebaseAuth.ts               NF-500 Firebase login helpers
    AuthScreen.tsx                NF-510 older local auth screen
    auth.ts                       NF-510 older local auth storage/helpers
    ScanToPunch.tsx               NF-600 scan/photo OCR punch import
    appleQuickSignIn.ts           NF-520 Apple quick sign-in flag/helper
  __tests__/                      NF-900 tests
  scripts/configure-firebase.sh   NF-950 Firebase setup helper
  ios/                            NF-800 iPhone/iPad native project
  android/                        NF-810 Android native project
  app.json                        NF-700 app metadata
  firebase.json                   NF-700 Firebase runtime settings
  package.json                    NF-700 scripts and dependencies
  SETUP.md                        Extra setup notes
```

## Run The App

Start the React Native server:

```sh
npm start
```

Run on iOS:

```sh
npm run ios
```

Run on Android:

```sh
npm run android
```

Run tests:

```sh
npm test
```

## Native Project Entry Points

Use these when opening the app in platform tools:

| Platform | Open this |
| --- | --- |
| iOS | `ios/TimecardApp.xcworkspace` |
| Android | `android/` |

## NF-820: Xcode and Android Studio Theme Cross-Reference

Use this section when you want the iOS and Android versions to match visually.

| Theme job | Xcode / iOS file | Android Studio / Android file | What to check |
| --- | --- | --- | --- |
| Open native project | `ios/TimecardApp.xcworkspace` | `android/` | Main native entry point for each IDE |
| App name | `ios/TimecardApp/Info.plist` | `android/app/src/main/res/values/strings.xml` | Display name shown by the operating system |
| App icon | `ios/TimecardApp/Images.xcassets/AppIcon.appiconset/` | `android/app/src/main/res/mipmap-*/ic_launcher*.png` | Home screen and launcher icon |
| Launch screen | `ios/TimecardApp/LaunchScreen.storyboard` | `android/app/src/main/res/values/styles.xml` | First screen while the app loads |
| Native app theme | `ios/TimecardApp/Info.plist` and `ios/TimecardApp/AppDelegate.swift` | `android/app/src/main/res/values/styles.xml` | Status bar, launch behavior, no-action-bar theme |
| Theme link | `UILaunchStoryboardName` in `Info.plist` | `android:theme="@style/AppTheme"` in `AndroidManifest.xml` | Where each platform connects to its theme |
| Permissions text | `ios/TimecardApp/Info.plist` | `android/app/src/main/AndroidManifest.xml` | Camera, photos, internet, and system permission copy |
| Firebase visual setup | `ios/TimecardApp/GoogleService-Info.plist` | `android/app/google-services.json` | Platform Firebase app identity |
| Main React Native styling | `App.tsx`, `src/FirebaseAuthScreen.tsx`, `src/ScanToPunch.tsx` | Same React Native files | Shared paper-card UI, colors, typography, buttons |

Quick searches:

```sh
rg "NF-820"
rg "UILaunchStoryboardName" ios/TimecardApp/Info.plist
rg "android:theme" android/app/src/main/AndroidManifest.xml
rg "AppTheme" android/app/src/main/res/values/styles.xml
rg "LaunchScreen" ios/TimecardApp
```

### Xcode Theme Notes

| NFCS code | File | Purpose |
| --- | --- | --- |
| NF-820-IOS-1 | `ios/TimecardApp.xcworkspace` | Open this in Xcode, not the `.xcodeproj`, when working with CocoaPods |
| NF-820-IOS-2 | `ios/TimecardApp/Info.plist` | App display name, launch storyboard, permission text, status-bar behavior |
| NF-820-IOS-3 | `ios/TimecardApp/LaunchScreen.storyboard` | iOS launch/splash screen layout and background |
| NF-820-IOS-4 | `ios/TimecardApp/Images.xcassets/AppIcon.appiconset/` | iOS app icon set |
| NF-820-IOS-5 | `ios/TimecardApp/AppDelegate.swift` | Native iOS app startup and React Native boot |

Current iOS theme hooks:

```text
Info.plist -> UILaunchStoryboardName -> LaunchScreen
LaunchScreen.storyboard -> systemBackgroundColor + TimecardApp launch labels
App.tsx -> StatusBar barStyle="dark-content"
```

### Android Studio Theme Notes

| NFCS code | File | Purpose |
| --- | --- | --- |
| NF-820-AND-1 | `android/` | Open this folder in Android Studio |
| NF-820-AND-2 | `android/app/src/main/AndroidManifest.xml` | Connects the app to `@style/AppTheme` and launcher icons |
| NF-820-AND-3 | `android/app/src/main/res/values/styles.xml` | Android native theme definition |
| NF-820-AND-4 | `android/app/src/main/res/values/strings.xml` | Android app display name |
| NF-820-AND-5 | `android/app/src/main/res/mipmap-*/` | Android launcher and round launcher icons |

Current Android theme hooks:

```text
AndroidManifest.xml -> android:theme="@style/AppTheme"
styles.xml -> AppTheme -> Theme.AppCompat.DayNight.NoActionBar
mipmap-* -> ic_launcher and ic_launcher_round
```

## NFCS Editing Rule

When you add a new major feature, add a new NFCS number here. Keep the pattern simple:

```text
NF-100 App start
NF-200 Profiles
NF-300 Punch clock
NF-400 Schedule
NF-500 Auth
NF-600 Scanning
NF-700 Config
NF-800 Native projects
NF-820 Native themes
NF-900 Tests
```

That way every important code area has a short searchable label.
