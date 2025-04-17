extends Node2D

# A cooked meal that is ready to eat. Alternate version of frozen meal.

# Set object name for tags
var object_name = "cooked-meal"

func _enter_tree() -> void:
	# Are we being held?
	var holder = get_parent() as Person
	
	# Activate actions that are relevant
	%Eat.active = holder != null
	if holder:
		# If being held, only the person holding it can eat it
		%Eat.tags = [holder.full_name]
	else:
		%PickUp.stock = 1


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# Selected to be picked up. Disable others from picking it up.
		person.do_walk_to(self)
		person.do_grab(self)
	elif action == %Eat:
		# Straightforward, the person eats the meal
		person.do_wait(4.0, 5.0)
		person.do_destroy()
		person.do_reward(action.advert)
		
		# They will want to use the toilet more after
		person.do_reward({"clean": -0.08})
