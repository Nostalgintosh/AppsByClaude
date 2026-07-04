# 🌙 Dark Mode Cheat Sheet — TimecardApp

All line numbers refer to **`App.tsx`**. Light mode is unchanged; every dark color
lives in a `…Dark` style that only applies when dark mode is ON.

---

## 1. How the system works (the one pattern to know)

Every element looks like this:

```jsx
<View style={[st.card, darkMode && st.cardDark]}>
//            ^ light style   ^ only used when dark mode is on
```

- `st.card` → the normal (light) look.
- `darkMode && st.cardDark` → when `darkMode` is true, `st.cardDark` is layered on top and **wins**.
- So: **to change a dark color, edit the `…Dark` style.** To change the light color, edit the base style.

**Where the switch lives:**

| What | Line | Notes |
|---|---|---|
| `darkMode` state (default OFF) | **732** | `useState(false)` |
| The toggle in Settings (what flips it) | **2505** | `onPress={() => setDarkMode(v => !v)}` |
| Loads saved value per profile | 853 | `setDarkMode(d.darkMode ?? false)` |
| `ThemeContext` (feeds sub-components) | 99 | — |
| Theme providers (3 layouts) | 1469, 1628, 1751 | all = `value={darkMode}` |

---

## 2. "I want to change…" — quick lookup

| You want to change | Edit this style | Line | Current dark value |
|---|---|---|---|
| **All normal text** (brightness) | `textDark` | **2977** | `#f3f5f7` |
| **All gray/secondary text** | `textMutedDark` | **2978** | `#aeb6c2` |
| **Screen background** (deepest) | `safeDark` | 2966 | `#111317` |
| Scroll area behind cards | `mainScrollDark` | 2971 | `#15181d` |
| **The big content cards** | `cardDark` | **3127** | `#20242b` |
| Info/earnings rows inside cards | `eRowDark` | 3189 | `#171a20` |
| Top header bar | `appHeaderDark` | 3025 | `#171a20` |
| The PUNCH/HISTORY/etc. tab strip | `tabCardRowDark` / `tabCardDark` / `tabCardActiveDark` | 3083 / 3098 / 3111 | `#111317` / `#1d2229` / `#262c35` |
| Green dollar amounts | `amtGreenDark` | 3203 | `#7bd389` |
| Blue (weekly) amount | `amtBlueDark` | 3204 | `#79b6f2` |

---

## 3. Full color reference (grouped)

### Shared text (changing these recolors text app-wide)
| Style | Line | Value | Used for |
|---|---|---|---|
| `textDark` | 2977 | `#f3f5f7` | primary text |
| `textMutedDark` | 2978 | `#aeb6c2` | labels, captions, secondary text |

### Surfaces / backgrounds (darkest → lightest = furthest back → most raised)
| Style | Line | Value |
|---|---|---|
| `safeDark` | 2966 | `#111317` |
| `tabCardRowDark` | 3083 | `#111317` |
| `foldSidebarDark` | 3295 | `#14171c` |
| `mainScrollDark` / `folderBodyDark` / `foldContentPanelDark` | 2971 / 3205 / 3466 | `#15181d` |
| `eRowDark`, `statusOffDark`, `shiftChipDark`, `earningsTwoDark`, `sessionRowDark`, `podCardDark`, `accountInfoBoxDark` | 3189, 3195, 3198, 3202, 3209, 3228, 3528 | `#171a20` |
| `inputDark` (text fields) | 3670 | `#171a20` |
| `profileCardDark` / `widgetDark` | 2997 / 3214 | `#1c2027` |
| `tabCardDark` | 3098 | `#1d2229` |
| `cardDark` / `breakChipDark` / `widgetBreakDark` / `foldEmployeeCardDark` | 3127, 3199, 3219, 3345 | `#20242b` |
| `profileRowDark` | 3006 | `#252a32` |
| `tabCardActiveDark` / `foldTabItemActiveDark` / `radioRowActiveDark` | 3111, 3447, 3674 | `#262c35` |
| `schedTypeBtnActiveDark`, `rotPeriodBtnActiveDark`, `podDayChipActiveDark` (selected buttons) | 3225, 3227, 3232 | `#3a3f48` |
| `taxBoxDark` | 3709 | `#121820` |

### Colored status backgrounds (tinted)
| Style | Line | Value | Meaning |
|---|---|---|---|
| `statusOnDark` / `widgetStatusOnDark` | 3196 / 3215 | `#16241a` | on duty (green tint) |
| `onBreakBoxDark` / `widgetStatusBreakDark` | 3200 / 3217 | `#2a2412` | on break (amber tint) |
| `widgetStatusOffDark` | 3216 | `#2f1917` | off duty (red tint) |

### Borders (most common first)
| Value | Used by | Example line |
|---|---|---|
| `#343b46` | subtle borders / dividers | eRowDark 3189 |
| `#39404a` | standard borders | cardDark 3127 |
| `#4b5563` | lighter borders (inputs, status, header) | statusOffDark 3195 |

### Accent text (kept vivid on purpose)
| Style | Line | Value | Meaning |
|---|---|---|---|
| `amtGreenDark`, `statusOnLabelDark`, `dayTotalDark`, `sessionEarningsDark`, `schedToggleOnDark` | 3203, 3197, 3208, 3210, 3235 | `#7bd389` | green (money / "on") |
| `amtBlueDark` | 3204 | `#79b6f2` | blue (weekly money) |
| `onBreakInkDark` | 3201 | `#e8c07a` | amber (break) |
| `taxBoxStateDark` | 3711 | `#93c5fd` | tax box accent |

### Light-mode constants (for reference — NOT dark)
| Const | Line | Value |
|---|---|---|
| `CREAM` (paper bg) | 2954 | `#f2efe6` |
| `INK` (normal text) | 2955 | `#1a1a1a` |
| `FONT` | 2958 | Courier/monospace |

> ⚠️ `widgetBtnDark` (line 3733) is **NOT** a dark-mode style — despite the name it's the
> light-mode style for a button with dark-colored text. Leave it alone.

---

## 4. How to change a color (step by step)

1. Open `App.tsx`, jump to the line from the tables above.
2. Edit the hex value, e.g. make the cards a bit darker:
   ```jsx
   cardDark:{backgroundColor:'#20242b', borderColor:'#39404a'},   // before
   cardDark:{backgroundColor:'#1a1d23', borderColor:'#2c323b'},   // after
   ```
3. Save, then rebuild + reinstall (section 6).

**Color tips:** bigger/brighter hex = lighter. A surface "ramp" that reads well:
`#111317` (back) → `#171a20` → `#20242b` → `#262c35` (raised). Keep text ≥ `#aeb6c2`
for readability on these.

---

## 5. An element is still light / I want to theme something new

1. **Add a dark variant** next to the others in the `st = StyleSheet.create({…})` block:
   ```jsx
   myThingDark: {backgroundColor:'#171a20', borderColor:'#39404a'},
   ```
2. **Wire it** at the element — turn the single style into an array:
   ```jsx
   <View style={st.myThing}>                         // before
   <View style={[st.myThing, darkMode && st.myThingDark]}>   // after
   ```
3. **⚠️ Scope check (this is what broke launch before):** `darkMode` only exists inside the
   main `App` component and components that have `const darkMode = React.useContext(ThemeContext);`
   at the top (Card 2771, ERow 2778, AccountSection 2836, NewProfileModal 2887).
   If you add `darkMode &&` to **any other** component, add that `useContext` line to it first —
   otherwise the app **crashes on launch** with `ReferenceError: Property 'darkMode' doesn't exist`.

---

## 6. Build & install after changes

```sh
cd ~/Documents/TimecardApp

# 1) TYPE-CHECK FIRST — catches the "doesn't exist" crash before you ever build it.
#    (The release build does NOT type-check, so skipping this can ship a crash.)
npx --no-install tsc --noEmit -p tsconfig.json        # want: no App.tsx errors

# 2) Build (arm64 only = fast)
export JAVA_HOME="/Applications/Android Studio.app/Contents/jbr/Contents/Home"
export ANDROID_HOME="$HOME/Library/Android/sdk"
cd android && ./gradlew :app:assembleRelease -PreactNativeArchitectures=arm64-v8a --no-daemon

# 3) Install over the existing app (keeps all your data)
$ANDROID_HOME/platform-tools/adb install -r app/build/outputs/apk/release/app-release.apk
```

Then open the app on the phone. If a screen still looks wrong, find the element's style
name, check section 3 for its `…Dark` line, and tweak.

---

## 7. Map of where each screen's colors live

| Screen / area | Style lines |
|---|---|
| Login / auth | uses base styles (mostly light by design) |
| Profile picker | `profileCardDark` 2997, `profileRowDark` 3006 |
| Header + tab strip | `appHeaderDark` 3025, `tabCard*Dark` 3083–3111 |
| **Punch tab** | 3195–3204 (status boxes, chips, earnings tiles) |
| **History tab** | 3205–3211 (folders, day rows, sessions) |
| **Schedule tab** | 3221–3237 (calendar, type buttons, pods, week rows) |
| **Settings tab** | 3488, 3528, 3670–3711 (inputs, radios, dropdowns, tax box) |
| Floating widget | 3214–3219 |
| Foldable (unfolded) layout | 3295–3466 |
