extends StaticBody2D

# A photocopier that duplicates pieces of paper. More paper = more work achieved

func plan(person: Person, action: Action) -> void:
	if action == %CopySomething:
		# The person is holding paper and wants to photocopy it
		person.do_walk_to(self)
		person.do_start_use(self)
		
		# Put paper in
		person.do_destroy()
		person.do_wait(6.0, 8.0)
		
		# Get paper out
		person.do_gain("res://objects/Paper.tscn")
		
		# Drop it on the floor (can only hold one thing)
		person.do_drop()
		
		# Get more paper out
		person.do_gain("res://objects/Paper.tscn")
		person.do_stop_use(self)
		
		# Congrats
		person.do_reward(action.advert)
		
		# Once used, the copier has a cooldown period to prevent people from
		# copying paper indefinitely
		%CopySomething.active = false
		%Timer.start()


func _on_timer_timeout() -> void:
	# Copier is ready to copy again
	%CopySomething.active = true
