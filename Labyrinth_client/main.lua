local composer = require "composer"
local thread = require 'thread'

display.setStatusBar(display.HiddenStatusBar)

thread.instance = thread:init()
thread.instance:start()

composer.gotoScene("menu")