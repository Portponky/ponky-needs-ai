extends Node2D

var object_name := "coffee"


func _enter_tree() -> void:
	var holder = get_parent() as Person
	
	%PickUp.active = holder == null
	%Drink.active = holder != null
	if holder:
		%Drink.tags = [holder.full_name]


func plan(person: Person, action: Action) -> void:
	if action == %PickUp:
		person.do_walk_to(global_position)
		person.do_grab(self)
		%PickUp.active = false
	elif action == %Drink:
		person.do_wait(2.0, 3.0)
		if randf() < 0.4:
			person.do_destroy()
		person.do_reward(action.advert)
		person.do_reward({"clean": -0.05})


func _on_timer_timeout() -> void:
	# coffee is cold now
	%Drink.active = false
	object_name = "cold-coffee"
