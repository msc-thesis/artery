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

	c5 = storyboard.Coord(2000, 1500)
	c6 = storyboard.Coord(4000, 1500)
	c7 = storyboard.Coord(4000, 2500)
	c8 = storyboard.Coord(2000, 2500)
	
	polygon_cond_2 = storyboard.PolygonCondition([c5, c6, c7, c8])
	#car_cond_2 = storyboard.CarSetCondition({"0"})
	#and1 = storyboard.AndCondition(polygon_cond_2, car_cond_2)
	
	fog_effect = storyboard.SignalEffect("fog")

	story3 = storyboard.Story(polygon_cond_2, [fog_effect])


	board.registerStory(story)
	board.registerStory(story2)
	board.registerStory(story3)

	print("Stories loaded!")
