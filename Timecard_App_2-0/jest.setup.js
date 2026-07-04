/* eslint-env jest */

jest.mock('@react-native-async-storage/async-storage', () => {
  const store = new Map();

  return {
    clear: jest.fn(async () => store.clear()),
    getAllKeys: jest.fn(async () => Array.from(store.keys())),
    getItem: jest.fn(async key => store.get(key) ?? null),
    removeItem: jest.fn(async key => store.delete(key)),
    removeMany: jest.fn(async keys => keys.forEach(key => store.delete(key))),
    setItem: jest.fn(async (key, value) => store.set(key, value)),
  };
});

jest.mock('react-native-safe-area-context', () => {
  const React = require('react');
  const {View} = require('react-native');

  return {
    SafeAreaProvider: ({children}) => React.createElement(View, null, children),
    SafeAreaView: View,
  };
});

jest.mock('react-native-biometrics', () => {
  class ReactNativeBiometrics {
    isSensorAvailable = jest.fn(async () => ({available: false}));
    simplePrompt = jest.fn(async () => ({success: false}));
  }

  return {
    __esModule: true,
    default: ReactNativeBiometrics,
    BiometryTypes: {
      Biometrics: 'Biometrics',
      FaceID: 'FaceID',
      TouchID: 'TouchID',
    },
  };
});

jest.mock('react-native-image-picker', () => ({
  launchCamera: jest.fn(),
  launchImageLibrary: jest.fn(),
}));

jest.mock('react-native-text-recognition', () => ({
  __esModule: true,
  default: {
    recognize: jest.fn(async () => []),
  },
}));

jest.mock('@invertase/react-native-apple-authentication', () => {
  const React = require('react');
  const {View} = require('react-native');
  const AppleButton = props => React.createElement(View, props);
  AppleButton.Style = {BLACK: 'Black'};
  AppleButton.Type = {CONTINUE: 'Continue'};

  return {
    AppleButton,
    appleAuth: {
      isSupported: false,
      Error: {CANCELED: '1001'},
      Operation: {LOGIN: 1},
      performRequest: jest.fn(),
    },
  };
});

jest.mock('@react-native-firebase/auth', () => {
  const authInstance = {
    currentUser: null,
    onAuthStateChanged: jest.fn(callback => {
      callback(null);
      return jest.fn();
    }),
    signInWithEmailAndPassword: jest.fn(),
    createUserWithEmailAndPassword: jest.fn(),
    sendPasswordResetEmail: jest.fn(),
    signInWithCredential: jest.fn(),
    signOut: jest.fn(),
  };
  const auth = jest.fn(() => authInstance);
  auth.GoogleAuthProvider = {credential: jest.fn()};
  auth.AppleAuthProvider = {credential: jest.fn()};

  return {
    __esModule: true,
    default: auth,
  };
});

jest.mock('@react-native-firebase/auth/lib/modular', () => {
  const authInstance = {
    currentUser: null,
  };

  return {
    __esModule: true,
    getAuth: jest.fn(() => authInstance),
    onAuthStateChanged: jest.fn((_auth, callback) => {
      callback(null);
      return jest.fn();
    }),
    signInWithEmailAndPassword: jest.fn(async () => ({user: authInstance.currentUser})),
    createUserWithEmailAndPassword: jest.fn(async () => {
      authInstance.currentUser = {
        uid: 'test-user',
        email: 'test@example.com',
        displayName: null,
        photoURL: null,
        providerData: [],
      };
      return {user: authInstance.currentUser};
    }),
    sendPasswordResetEmail: jest.fn(async () => undefined),
    signInWithCredential: jest.fn(async () => ({user: authInstance.currentUser})),
    signOut: jest.fn(async () => {
      authInstance.currentUser = null;
    }),
    updateProfile: jest.fn(async (user, patch) => Object.assign(user, patch)),
  };
});

jest.mock('@react-native-google-signin/google-signin', () => ({
  GoogleSignin: {
    configure: jest.fn(),
    hasPlayServices: jest.fn(),
    signIn: jest.fn(),
    getTokens: jest.fn(),
    signOut: jest.fn(),
  },
}));
