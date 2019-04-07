# EncryptedPad
Simple command line program for jotting down notes which will be encrypted using AES256.

## Usage
To encrypt text to a file, e.g. "creativetitle":
```bash
./encrn -e creativetitle
```
This will open an editor to enter your notes. Press **F5** to save whatever your brain produced and quit the editor.
You will then be asked to enter a password used for encryption.

To decrypt and edit a file, e.g. "creativetitle_2":
```bash
./encrn -d creativetitle_2
```
This will open an editor you can use to view and edit your file after entering the correct password. *If a wrong password is entered, gibberish will be displayed.* Press **F5** to save and quit.

## ToDos
* The editor could be a bit more civilized
* Possibility to discard changes done in decrpytion mode
