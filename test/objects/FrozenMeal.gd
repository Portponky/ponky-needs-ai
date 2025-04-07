extends Node2D

# A frozen meal that must be heated to be edible.

# Set object name for tags
var object_name = "frozen-meal"

func _enter_tree() -> void:
	# Are we being held? If so, can't be picked up
	var holder = get_parent() as Person
	%PickUp.active = holder == null


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# Person wants the frozen meal, so they grab it
		person.do_walk_to(self)
		person.do_grab(self)
		%PickUp.active = false
		
		# Now they must figure out how to heat it for the reward
