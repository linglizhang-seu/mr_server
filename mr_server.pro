QT -= gui
QT += sql network

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH+=./include/hiredis
SOURCES += \
        basic_c_fun/basic_surf_objs.cpp \
        basic_c_fun/v3d_message.cpp \
        main.cpp \
        messageserver.cpp \
        messagesocket.cpp \
        neuron_editing/apo_xforms.cpp \
        neuron_editing/global_feature_compute.cpp \
        neuron_editing/neuron_format_converter.cpp \
        neuron_editing/neuron_sim_scores.cpp \
        neuron_editing/neuron_xforms.cpp \
        neuron_editing/v_neuronswc.cpp \
        tcpsocket.cpp \
        utils.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    basic_c_fun/basic_surf_objs.h \
    basic_c_fun/v3d_basicdatatype.h \
    basic_c_fun/v3d_message.h \
    basic_c_fun/v_neuronswc.h \
    messageserver.h \
    messagesocket.h \
    neuron_editing/apo_xforms.h \
    neuron_editing/global_feature_compute.h \
    neuron_editing/neuron_format_converter.h \
    neuron_editing/neuron_sim_scores.h \
    neuron_editing/neuron_xforms.h \
    neuron_editing/v_neuronswc.h \
    tcpsocket.h \
    utils.h
