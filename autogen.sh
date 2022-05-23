#!/bin/sh
cd `dirname $0`
mkdir -p config m4
autoreconf --force --install -I config -I m4