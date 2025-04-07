extends StaticBody2D

# A filing cabinet into which papers are placed. It's an office job!

func plan(person: Person, action: Action) -> void:
	if action == %FilePaper:
		# This person is holding paper and is going to "place it" into the cabinet
		# In reality, the paper just gets destroyed.
		person.do_walk_to(self)
		person.do_start_use(self)
		person.do_wait(1.0, 2.0)
		person.do_destroy()
		person.do_stop_use(self)
		person.do_reward(action.advert)
