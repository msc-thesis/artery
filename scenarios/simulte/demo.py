#!/usr/bin/env python

# reference variable to Storyboard Omnet++ module: board
import storyboard
import timeline

print ("demo.py successfully imported...")

def createStories(board):
	# Create coordinates needed for the PolygonCondition
	coord0 = storyboard.Coord(0.0, 0.0)
	coord1 = storyboard.Coord(2000.0, 0.0)
	coord2 = storyboard.Coord(2000.0, 1500.0)
	coord3 = storyboard.Coord(0.0, 1500.0)

	# Create PolygonCondition
	polygon_cond = storyboard.PolygonCondition([coord0, coord1, coord2, coord3])

	# Create TimeCondition
	time_cond = storyboard.TimeCondition(timeline.milliseconds(5000))

	# Create CarSetCondition
	cars_cond = storyboard.CarSetCondition({"0", "1", "2"})
	
	signal_effect = storyboard.SignalEffect("slow down")

	impact_reduction_effect = storyboard.SignalEffect("irc")

	# Create AndConditions
	and0 = storyboard.AndCondition(polygon_cond, time_cond)
	and1 = storyboard.AndCondition(and0, cars_cond)

	# Create Story
	story = storyboard.Story(and1, [impact_reduction_effect])


	car_cond = storyboard.CarSetCondition({"4"})
	traction_loss = storyboard.SignalEffect("traction loss")
	story2 = storyboard.Story(car_cond, [traction_loss])

	# Register Stories at the Storyboard
	board.registerStory(story)
	board.registerStory(story2)

	print("Stories loaded!")
