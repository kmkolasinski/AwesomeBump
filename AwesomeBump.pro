TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += Sources

!equals(PWD, $$OUT_PWD) {
	error("!!! Please, run qmake in the root directory of the project - AB will create its own workdir automatically.")
}
