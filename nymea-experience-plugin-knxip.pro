TEMPLATE = subdirs

SUBDIRS += libnymea-knx plugin-experience knxipinterface

plugin-experience.depends = libnymea-knx
knxipinterface.depends = libnymea-knx
