extends Node2D

# A sandwich. Not as good as a cooked meal but ok as a snack.

func _enter_tree() -> void:
	# Are we being held?
	var holder = get_parent() as Person
	
	# Activate actions that are relevant
	%PickUp.active = holder == null
	%Eat.active = holder != null
	if holder:
		# If being held, only the person holding it can eat
		%Eat.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# This person is going to grab the sandwich
		person.do_walk_to(self)
		person.do_grab(self)
		
		# They get a small reward for picking up the sandwich
		person.do_reward({"social": 0.08})
		%PickUp.active = false
	
	elif action == %Eat:
		# Wander around and eat the sandwich
		person.do_loiter(1.2)
		person.do_destroy()
		person.do_reward(action.advert)
