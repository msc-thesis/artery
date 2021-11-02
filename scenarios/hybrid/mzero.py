#!/usr/bin/env python3

import storyboard

print ("mzero.py successfully imported...")

def createStories(board):
	c1 = storyboard.Coord(4000, 1500)
	c2 = storyboard.Coord(6000, 1500)
	c3 = storyboard.Coord(6000, 2500)
	c4 = storyboard.Coord(4000, 2500)
	
	polygon_cond = storyboard.PolygonCondition([c1, c2, c3, c4])
	
	fog_effect = storyboard.SignalEffect("fog")

	story = storyboard.Story(polygon_cond, [fog_effect])

	board.registerStory(story)

	print("Stories loaded!")
