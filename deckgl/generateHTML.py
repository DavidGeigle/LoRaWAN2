"""
HexagonLayer
==============

Personal injury road accidents in GB from 1979.

The layer aggregates data within the boundary of each hexagon cell.

This example is adapted from the deck.gl documentation.
"""

import pydeck as pdk

HEATMAP_LAYER_DATA = (
    "data2.csv"  # noqa
)

# Define a layer to display on a map
layer = pdk.Layer(
    "HeatmapLayer",
    HEATMAP_LAYER_DATA,
    get_position=["longitude", "latitude"],
    auto_highlight=True,
    aggregation='"MEAN"',
    #get_weight="weight",
    pickable=True,
)

# Set the viewport location
view_state = pdk.ViewState(
    longitude=8.200262, latitude=48.105194, zoom=14, min_zoom=1, max_zoom=20, pitch=40.5, bearing=-27.36,
)

# Render
r = pdk.Deck(layers=[layer], initial_view_state=view_state, mapbox_key="pk.eyJ1IjoiYXJjYXhjIiwiYSI6ImNrYjd4bmNoNjBhYTEyd3FocTBnYzFoZ24ifQ.i2OEIQFKyieMxuUhq-s9Xw",)
r.to_html("heatmap.html", notebook_display=False)
