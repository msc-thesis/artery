#! /bin/sh
DIR=`dirname $0`
opp_run -l$DIR/../../base/miximbase -l$DIR/../../modules/miximmodules -n $DIR/../../examples:$DIR/../../base:$DIR/../../modules $*

