TEMPLATE = subdirs

SUBDIRS += libnymea-knx plugin-experience knxipinterface

plugin-experience.depends = libnymea-knx
gateway.depends = libnymea-knx
