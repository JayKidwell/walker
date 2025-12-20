
image the rpi
    hostname is the sequential 'rpi#', un: jay, pw: rpi8

setup rpi
    enable i2c, >sudo raspi-config

Make changes from the git folder in vs-code
    c:\git\walker\source

use winscp to transfer file to the rpi
    ~/walker

ssh to rpi from vsc terminal
    > ssh jay@rpi8
    > password rpi8

setup environment
    create venv
        > cd ~/your_project
        > python3 -m venv venv
    initialize at the beginning of each session
        > source venv/bin/activate
    when done to deactivate
        > deactivate
    pip
        upgrade pip outside venv
            > python3 -m pip install --upgrade pip
        upgrade pip inside venv
            > pip install --upgrade pip
        Install packages: 
            > pip install package_name (while activated)
        Save requirements: 
            > pip freeze > requirements.txt
        Install from requirements: 
            > pip install -r requirements.txt
        List installed packages
            > pip list
    update system
        > sudo apt update
        > sudo apt upgrade python3

detect i2c devices
    > i2cdetect -y 1




