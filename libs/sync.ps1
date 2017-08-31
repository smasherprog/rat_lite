cd .. 

git subtree pull --prefix=libs/input_lite https://github.com/smasherprog/input_lite master --squash
git subtree push --prefix=libs/input_lite https://github.com/smasherprog/input_lite master --squash

git subtree pull --prefix=libs/clipboard_lite https://github.com/smasherprog/clipboard_lite master --squash
git subtree push --prefix=libs/clipboard_lite https://github.com/smasherprog/clipboard_lite master --squash

git subtree pull --prefix=libs/screen_capture_lite https://github.com/smasherprog/screen_capture_lite master --squash
git subtree push --prefix=libs/screen_capture_lite https://github.com/smasherprog/screen_capture_lite master --squash

git subtree pull --prefix=libs/websocket_lite https://github.com/smasherprog/websocket_lite master --squash
git subtree push --prefix=libs/websocket_lite https://github.com/smasherprog/websocket_lite master --squash

git subtree pull --prefix=libs/zlib https://github.com/madler/zlib v1.2.11 --squash
git subtree pull --prefix=libs/libjpeg-turbo https://github.com/libjpeg-turbo/libjpeg-turbo 1.5.2 --squash

cd libs 