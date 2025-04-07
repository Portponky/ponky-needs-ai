extends Node2D

# A hidden action to attract people to the bathroom.
# We don't want a person selecting a toilet from half way across the map and then
# walking straight into that cubicle and waiting for it to be free (weird)...
# Instead, the toilets are only advertised at short range, and we use this action
# to bring people into the bathroom foyer to wait for a cubicle from there.

# This might be better achieved by tagging the people with which area of the
# building they are in, and having standardized beacons attracting them to
# different functional areas. Once in the area, more specific actions become
# available through the use of tags.

func _ready() -> void:
	# We don't want to the beacon to be visible to the player
	$Sprite.visible = false


func plan(person: Person, action: Action) -> void:
	if action == %Beacon:
		# This person wants to use the bathroom, but they might be far away
		person.do_drop() # Don't carry food or work into the bathroom
		person.do_walk_to(self)
		person.do_loiter(0.2)
