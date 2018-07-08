# Piepmatz
Piepmatz is a Twitter client for Sailfish OS

## Author
Sebastian J. Wolf [sebastian@ygriega.de](mailto:sebastian@ygriega.de)

## License
Licensed under GNU GPLv3

## Keyboard Shortcuts
Some Sailfish OS devices feature a physical keyboard. Piepmatz is even easier to use with them using shortcuts:
- Generic
  - Up/down: scroll
  - Page up/down: scroll faster
  - t/b: Scroll to top / bottom
  - Left/Esc: Back
- Only on main screen:
  - 1-6: Switch tabs (timeline, notifications, search etc.)
  - n: New tweet
  - r: Refresh
  - s: Settings
  - a: About
- On tweet details view:
  - Return/Enter: Display thread

## Build
Simply clone this repository, but be sure to clone it with `--recursive` as Piepmatz uses Git Submodules.  

Just use the project file `harbour-piepmatz.pro` to import the sources in your SailfishOS IDE. To build and run Piepmatz or an application which is based on Piepmatz, you need to create the file `harbour-piepmatz/src/o2/o1twitterglobals.h` (you can copy the sample from `harbour-piepmatz/src/o2/o1twitterglobals.h.sample`) and enter the required constants in the following format:
```
#ifndef O1TWITTERGLOBALS_H
#define O1TWITTERGLOBALS_H
const char TWITTER_CLIENT_ID[]                    = "abcdef";
const char TWITTER_CLIENT_SECRET[]                = "ghijkl";
const char TWITTER_STORE_DEFAULT_ENCRYPTION_KEY[] = "mnopqr";
#endif // O1TWITTERGLOBALS_H
```

You get the Twitter client ID and client secret as soon as you've registered your own application on [dev.twitter.com](https://dev.twitter.com/). The default encryption key is only used in case Piepmatz is unable to determine a unique encryption key from the user's device. Under normal circumstances, Piepmatz uses an encryption key which was generated automatically. This key is used to encrypt the user's generated Twitter oAuth token (not the username/password!) on the user's device. Please use a password generator to generate the default key for your application.

Piepmatz also needs the package `openssl-devel` to compile properly. You can install it on your build target using the Control Centre in your SailfishOS IDE.

## Credits
This project uses
- OAuth for Qt, by Akos Polster. Available on [GitHub.com](https://github.com/pipacs/o2) - Thanks for making it available under the conditions of the BSD-2-Clause license! Details about the license of OAuth for Qt in [its license file](src/o2/LICENSE).
- twitter-text, copyright 2017 Twitter, Inc and other contributors. Available on [GitHub.com](https://github.com/twitter/twitter-text) - Thanks for making it available under the conditions of the Apache License, Version 2.0!  Details about the license of twitter-text in [its license file](qml/js/LICENSE-twitter-text).
- Emoji parsing and artwork by [Twitter Emoji (Twemoji)](http://twitter.github.io/twemoji/), copyright 2017 Twitter, Inc and other contributors, Code licensed under the [MIT License](http://opensource.org/licenses/MIT), Graphics licensed under [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/)
- HTML parsing by [QGumboParser](https://github.com/lagner/QGumboParser), copyright 2017 Sergey Lagner, Code licensed under the [MIT License](http://opensource.org/licenses/MIT), uses [Gumbo](https://github.com/google/gumbo-parser), copyright 2017 Google, licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) 
- ipinfo.io to determine your location.

Piepmatz was translated to several languages. Thanks to all contributors!
- Dutch: [Heimen Stoffels](https://www.transifex.com/user/profile/Vistaus/), [Nathan Follens](https://www.transifex.com/user/profile/pljmn/) and [d9h02f](https://github.com/d9h02f)
- Finnish: [Ari Järviö](https://www.transifex.com/user/profile/ari_jarvio/) and [Termitebug](https://www.transifex.com/user/profile/Termitebug/)
- French: [Alain Devarieux](https://www.transifex.com/user/profile/aldevar/), [lutinotmalin](https://www.transifex.com/user/profile/lutinotmalin/), [Sagittarii](https://www.transifex.com/user/profile/Sagittarii/) and [Quent-in](https://github.com/Quent-in)
- Hindi: [Parikshit Pahare](https://www.transifex.com/user/profile/p_pahare/)
- Hungarian: [Szabó G.](https://www.transifex.com/user/profile/leoka/)
- Italian: [fravaccaro](https://www.transifex.com/user/profile/ghostofasmile/)
- Japanese: [helicalgear](https://www.transifex.com/user/profile/helicalgear/)
- Polish: [atlochowski](https://www.transifex.com/user/profile/atlochowski/)
- Portuguese (Brazil): [Adriano C.](https://www.transifex.com/user/profile/adrianocolvero/) and [Rafael Vinicius Leite Mendes](https://www.transifex.com/user/profile/marreko/)
- Romanian: [horho mepuc](https://www.transifex.com/user/profile/horho/)
- Russian: [Алексей Дедун](https://www.transifex.com/user/profile/lewa/), [Юрий](https://www.transifex.com/user/profile/iAncelad/) and [Oleg Urzhumtcev](https://www.transifex.com/user/profile/NetBUG/)
- Slowenian: [Boštjan Štrumbelj](https://www.transifex.com/user/profile/sponka/)
- Spanish: [Fernando Lanero](https://www.transifex.com/user/profile/ferlanero/) and [Carmen F. B.](https://github.com/carmenfdezb)
- Swedish: [Åke Engelbrektson](https://github.com/eson57)

Code improvements by:
- [Rolf Eike Beer](https://github.com/DerDakon) 
- [Kay Stenschke](https://github.com/kstenschke)
