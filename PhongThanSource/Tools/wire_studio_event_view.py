"""Compatibility entry point: preserve separate Studio modules (19 Oct clarification)."""
from pathlib import Path
import runpy

runpy.run_path(str(Path(__file__).with_name('correct_event_gui_architecture.py')), run_name='__main__')
