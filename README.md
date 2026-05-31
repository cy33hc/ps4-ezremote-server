# PS4 ezRemote Server

This is a payload that runs in the background to allow installing/download PS4 packages from ftp/sftp, nfs, smb, webdav, http servers (rclone, MS IIS, apache, nginx, npxserve), github, archive.org and any direct http links.

This is mainly used with ezRemote Client which allows you to browse files on the remote servers and submit request to ezRemote Server for installing.

This payload is auto-loaded by ezRemote Client when it starts up or can be added to Goldhen payload loader.

## Install PS4 pkgs in the background
Use ezRemote client to connect to remote server and select pkg to install. You can then close ezRemote Client app and the package will continue installing in background. This is archieved with ezRemote Server payload. ezRemote Server runs in the background and acts as a proxy to the remote server. ezRemote Server must be started to enable backgroud installs
   - support background install from all remote servers that ezremote client supports. ftp/sftp, nfs, smb, webdav, http servers (rclone, MS IIS, apache, nginx, npxserve), github, archive.org and any direct http links.
   - support background install from file host like mediafire, google shared link, pixeldrain, real-debrid and all-debrid
   - does not support background install using the options "Disk Cache" or PKGs inside zip files
   - You can pause and resume installs
   - After restarting PS5, you can resume installs by first reloading ezRemote Server
   - From testing, background install continues even in rest mode (This isn't granteed). You may need to restart ezRemote Server payload before you can resume install
   - ezRemote Server payload can be stop/restarted from ezRemote Client settngs dialog.
   - Do not submit to many background installs, it can crash ezRemote Server.

## Download large files in the background
  - You can enable/disable background download in the Global settings
  - You can set the minimum file size where background download will use. Default is 1GB
  - In Global Settings, you can show the background download progress of all requested downloads
  - Does not support rest mode. Background downloads will stop in rest mode, but will be resumed when ezRemote Server is restarted.
  - If PS5 is restarted, background downloads will resume when ezRemote Server is restarted.
  - Updated the Web UI, so you can download files of File shares like mediafire, google shared link, pixeldrain, real-debrid and all-debrid or any direct links
