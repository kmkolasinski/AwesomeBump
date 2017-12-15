TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += AwesomeBumpGUI AwesomeBumpCLI

!equals(PWD, $$OUT_PWD) {
	error("!!! Please, run qmake in the root directory of the project - AB will create its own workdir automatically.")
}

AwesomeBumpGUI.file = Sources/AwesomeBumpGUI.pro
AwesomeBumpCLI.file = Sources/AwesomeBumpCLI.pro
