#!/usr/bin/env python3

import storyboard
import timeline

print ("mzero.py successfully imported...")

def createStories(board):
	time_cond = storyboard.TimeCondition(timeline.milliseconds(5000))
	cars_cond = storyboard.CarSetCondition({"0", "1", "2"})

	impact_reduction_effect = storyboard.SignalEffect("irc")

	and0 = storyboard.AndCondition(time_cond, cars_cond)

	story = storyboard.Story(and0, [impact_reduction_effect])


	c1 = storyboard.Coord(10000, 3000)
	c2 = storyboard.Coord(12000, 3000)
	c3 = storyboard.Coord(12000, 4500)
	c4 = storyboard.Coord(10000, 4500)
	polygon_cond = storyboard.PolygonCondition([c1, c2, c3, c4])

	traction_loss_effect = storyboard.SignalEffect("traction loss")

	story2 = storyboard.Story(polygon_cond, [traction_loss_effect])



	board.registerStory(story)
	board.registerStory(story2)

	print("Stories loaded!")
