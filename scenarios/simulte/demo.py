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
	cond0 = storyboard.PolygonCondition([coord0, coord1, coord2, coord3])

	# Create TimeCondition
	cond1 = storyboard.TimeCondition(timeline.milliseconds(1000))

	# Create CarSetCondition
	cond2 = storyboard.CarSetCondition({"0", "1", "2"})
	
	signal_effect = storyboard.SignalEffect("slow down")

	impact_reduction_effect = storyboard.SignalEffect("irc")

	# Create AndConditions
	and0 = storyboard.AndCondition(cond0, cond1)
	and1 = storyboard.AndCondition(and0, cond2)

	# Create Story
	story = storyboard.Story(and1, [impact_reduction_effect])

	# Register Stories at the Storyboard
	board.registerStory(story)

	print("Stories loaded!")
