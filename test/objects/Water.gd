extends Node2D

# Cup of water. Person can drink from it if holding it, but it randomly runs out.

func _enter_tree() -> void:
	# Are we being held?
	var holder = get_parent() as Person
	
	# Activate actions that are relevant
	%Drink.active = holder != null
	if holder:
		# If being held, only the person holding it can drink
		%Drink.tags = [holder.full_name]
	else:
		%PickUp.stock = 1


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# Selected to be picked up. Disable others from picking it up.
		person.do_walk_to(self)
		person.do_grab(self)
	
	elif action == %Drink:
		# Take a drink of water
		person.do_wait(2.0, 3.0)
		
		# Randomly choose if they drank all the water
		if randf() < 0.6:
			person.do_destroy()
		
		# Apply reward, and also make them need the bathroom a bit more
		person.do_reward(action.advert)
		person.do_reward({"clean": -0.03})
