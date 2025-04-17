extends Node2D

# It's a toilet. The people use it to feel more clean.
# It has a short cooldown before it can be used again (flush).

func plan(person: Person, action: Action) -> void:
	if action == %Poop:
		# Use the toilet
		person.do_start_use(self)
		person.do_wait(6.0, 8.0)
		person.do_stop_use(self)
		
		# Tidy up
		person.do_loiter(0.4)
		person.do_reward(action.advert)
		
		# Once this action is selected, wait a while before it's reselected
		%Timer.start()


func _on_timer_timeout() -> void:
	# Toilet is ready for use again
	%Poop.stock = 1
