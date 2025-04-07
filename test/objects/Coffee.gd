extends Node2D

# Cup of coffee. Overpowered drink that makes you look cool.
# After a short time become "cold coffee" that can be renewed in a microwave.

var object_name := "coffee"
var glugs := 2

func _enter_tree() -> void:
	# Are we being held?
	var holder = get_parent() as Person
	
	# Activate actions that are relevant
	%PickUp.active = holder == null
	%Drink.active = holder != null
	if holder:
		# If being held, only the person holding it can drink
		%Drink.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		# Selected to be picked up. Disable others from picking it up.
		person.do_walk_to(self)
		person.do_grab(self)
		%PickUp.active = false
	
	elif action == %Drink:
		# Take a glug of coffee
		person.do_wait(2.0, 3.0)
		
		# The cup only has two glugs, once it is empty get rid of it
		glugs -= 1
		if glugs == 0:
			person.do_destroy()
		
		# Give a reward, and also encourage toilet use
		person.do_reward(action.advert)
		person.do_reward({"clean": -0.05})


func _on_timer_timeout() -> void:
	# Coffee is cold now
	%Drink.active = false
	object_name = "cold-coffee"
