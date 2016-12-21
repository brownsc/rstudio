/*
 * TerminalBusyEvent.java
 *
 * Copyright (C) 2009-12 by RStudio, Inc.
 *
 * Unless you have received this program directly from RStudio pursuant
 * to the terms of a commercial license agreement with RStudio, then
 * this program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */
package org.rstudio.studio.client.workbench.views.terminal.events;

import com.google.gwt.event.shared.EventHandler;
import com.google.gwt.event.shared.GwtEvent;

/**
 * Sent if terminal is busy and showing a progress indicator would be
 * appropriate. For example, possibly running a batch job where knowing when
 * it becomes "not busy" would be helpful. This is different than, for example,
 * running a full-screen text editor, where showing a busy indicator would be
 * more annoying than helpful.
 */
public class TerminalBusyEvent extends GwtEvent<TerminalBusyEvent.Handler>
{
   public interface Handler extends EventHandler
   {
      void onTerminalBusy(TerminalBusyEvent event);
   }

   public TerminalBusyEvent(boolean isBusy)
   {
      isBusy_ = isBusy;
   }

   public boolean isBusy()
   {
      return isBusy_;
   }
   
   @Override
   protected void dispatch(Handler handler)
   {
      handler.onTerminalBusy(this);
   }

   @Override
   public Type<Handler> getAssociatedType()
   {
      return TYPE;
   }

   private final boolean isBusy_;

   public static final Type<Handler> TYPE = new Type<Handler>();
}
