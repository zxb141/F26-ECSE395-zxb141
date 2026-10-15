# Week 8 Project Log
**Team:** Zachary Blue, Aedan Reagans, Lucas Benson

## Monday, October 12th, 2026
**Individual:**
- Worked on the Verification and Validation Plans assignment (due tonight)
- Wrote the Verification Protocol (0.3) for the detection subsystem: 30 crawler-box approaches across 3 lanes, 20 adult walk-bys, 10 adult + baby, 10 min stationary, with pass/fail limits
- Wrote the Validation Plan sections: strategy (0.5.1), study design with Luis and Nary (0.5.2), metrics table (0.5.3), and acceptance criteria (0.5.4)
- Wrote the Validation Protocol (0.6): 30 min home visit with 5 scripted crawl runs, walk-throughs, 3 room audibility check, then 7 days of nightly texts and a final survey
- Built the Validation Tracing Matrix (0.7) linking each stakeholder need to its test, metric, and criteria.
- Wrote Risks, Safety, and Ethics (0.8), Iteration (0.9), and the Appendix (0.10) with the nightly text questions, final survey, and consent script
- Clarified the prototype stages in the plan: P1 paper box (done), P2 breadboard with jumper wires, P3 printed CAD box wired straight to the ESP32
- Noted we don't need a separate 5 V regulator since the SunFounder kit power supply module takes the 9 V battery and gives 5V / 3.3V
- Talked with Lucas in lab about the V&V plan and what's left to split up

**Group:**
- Finished and submitted the Verification and Validation Plans assignment
- Met in lab (Zach and Lucas) to go over the plan, settle the three prototype stages (paper box, breadboard, printed box with no breadboard), and confirm the kit power supply module replaces a separate 5 V regulator
- Aedan compiled the final doc and submitted it

## Wednesday, October 14th, 2026
**Individual:**
- Wrote the full prototype software (main.cpp): armed, motion alert, and contact alert states, PIR debounce, ultrasonic adult filter with a median-of-three function, 10 s alert timeout, LED and buzzer outputs, and CSV printing over serial for testing
- Got the breadboard running off the 9 V battery with Aedan
- Went through the instructor's feedback on the System Architecture doc and rewrote the electrical and software parts of Section D (Interface Definitions) as paragraphs that reference Figure 1 and Figure 2
- Pasted the code into the 10/14 meeting notes with instructions for Lucas and Aedan to test it in PlatformIO on the battery Friday since I will be out of town
- Submitted my GitHub link and reminded the team to version control throughout the week

**Group (zoom meeting):**
- Reviewed instructor feedback on the System Architecture and Prototype Plan and split up the fixes: Aedan redoing the functional diagram (arrows, darker lines, bigger labels, no crossing blocks), Lucas fixing the physical diagram and rewriting Section C and the mechanical part of D as paragraphs, me on the electrical and software parts of D, Aedan adding the revision block and resubmitting
- Split up the midterm presentation: Zach has storyline and evaluation plan part 1, Lucas has design overview and prototyping, Aedan has specifications and evaluation plan part 2
- Lucas finishing the CAD box before Friday so Aedan can print it in lab
- Lucas and Aedan will test the code on the breadboard with the battery Friday