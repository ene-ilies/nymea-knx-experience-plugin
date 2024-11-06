TEMPLATE = subdirs

SUBDIRS += libnymea-knx plugin-experience knxipinterface mdtglastaster2

plugin-experience.depends = libnymea-knx
knxipinterface.depends = libnymea-knx
