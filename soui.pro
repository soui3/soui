TEMPLATE = subdirs
TARGET = soui
CONFIG(x64){
TARGET = $$TARGET"64"
}
DEPENDPATH += .
INCLUDEPATH += .

include(cpy-cfg.pri)

SUBDIRS += third-part
SUBDIRS += utilities
SUBDIRS += soui-sys-resource
SUBDIRS += soui
SUBDIRS += components
SUBDIRS += demo
SUBDIRS += demo2

soui.depends += utilities soui-sys-resource
demo.depends += soui SkMatrix
