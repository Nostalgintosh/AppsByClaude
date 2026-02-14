## Promptlist for Time Card
**Date:** 2026-02-14
**Goal:** Create a timecard app for my workplaces.

### The Prompt Code
```xml
<role>
You are an expert React developer.
</role>

<context>
I am building a timecard app for tracking employee hours.
Current stack: React, Vite, Tailwind CSS.
</context>

<instructions>
1. Create a component that allows users to punch in and punch out.
2. Calculate total hours worked automatically.
3. Style it using dark mode.
</instructions>
```
### First Prompt: The core
Create a clock-in and clock-out feature that records each punch, displays total hours worked and estimated pay for the day, and stores the pay period (weekly, biweekly, semi-monthly, monthly, or yearly). Use the user's provided pay information to calculate daily pay. Ensure the feature works cross-platform (Android and iOS). Include selectable break timers for 15 minutes, 30 minutes, or 60 minutes.

### Secend Prompt: theme and profile
Allow users to create and delete profiles. Fix the Delete button so it functions correctly. When the user attempts to delete a profile, show a confirmation dialog that asks "Are you sure?" and delete only if the user confirms. Adjust the UI to a printer-paper aesthetic to give a corporate punch-card feel.

### Theid Prompt: Language settings
Add translation options in the configuration settings for English, Spanish, and Haitian Creole. Include additional regional languages where possible, for example Louisiana Creole and Pennsylvania Dutch.
