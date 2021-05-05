#!/bin/sh

# 既定のRubyを使用する場合
RUBY_PATH=$(which ruby)

# 既定以外の場所のRubyを使用する場合
# RUBY_PATH=/usr/local/bin/ruby

if [ -z $RUBY_PATH ]
then
  echo "Please specify the path of ruby."
  exit 1
fi

$RUBY_PATH ./bcdice-irc-server.rb $@
